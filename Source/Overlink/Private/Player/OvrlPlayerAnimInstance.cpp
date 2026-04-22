// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerAnimInstance.h"

// Internal
#include "Player/OvrlCharacterBase.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Player/Components/OvrlCameraComponent.h"

// Engine
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemGlobals.h"
#include "OvrlUtils.h"

UOvrlPlayerAnimInstance::UOvrlPlayerAnimInstance()
{
	StandingRootYawThreshold = FVector2D(-60.f, 60.f);
	CrouchingRootYawThreshold = FVector2D(-60.f, 60.f);
}

void UOvrlPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AOvrlCharacterBase>(GetOwningActor());

#if WITH_EDITOR
	const UWorld* World = GetWorld();

	if (IsValid(World) && !World->IsGameWorld() && !IsValid(PlayerCharacter))
	{
		// Use default objects for editor preview.
		PlayerCharacter = GetMutableDefault<AOvrlCharacterBase>();
	}
#endif

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerCharacter))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
	
	bIsPlayerValid = true;
}

void UOvrlPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ensureAlways(IsValid(PlayerCharacter));
	ensureAlways(IsValid(CharacterMovementComponent));
}

void UOvrlPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(CharacterMovementComponent))
		return;

	LocomotionAction = CharacterMovementComponent->GetLocomotionAction();
	LocomotionMode = CharacterMovementComponent->GetLocomotionMode();
	Stance = CharacterMovementComponent->GetStance();
	Gait = CharacterMovementComponent->GetGait();
	RightHandIKLocation = CharacterMovementComponent->GetRightHandIKLocation();
	LeftHandIKLocation = CharacterMovementComponent->GetLeftHandIKLocation();
}

void UOvrlPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (!CharacterMovementComponent || !PlayerCharacter)
		return;

	const FRotator ControlRotationGravityRel = UOvrlUtils::GetGravityRelativeRotation(PlayerCharacter->GetControlRotation(), CharacterMovementComponent->GetGravityDirection());
	const FRotator ComposedRotator = UKismetMathLibrary::ComposeRotators(ControlRotationGravityRel, FRotator(180.f, 0.f, 0.f));
	
	SpineRotation = FRotator(0.f, 0.f, ComposedRotator.Pitch);

	PitchAngle = -ComposedRotator.Pitch;

	// Movement
	bIsFalling = CharacterMovementComponent->IsFalling();
	bIsCrouching = CharacterMovementComponent->IsCrouching();
	bIsMoving = (Gait != OvrlGaitTags::Idle);
	bIsRunning = (Gait == OvrlGaitTags::Running);
	bIsSliding = (LocomotionAction == OvrlLocomotionActionTags::Sliding);
	bIsWallrunning = CharacterMovementComponent->IsWallrunning();
	bIsWallClinging = CharacterMovementComponent->IsWallClinging();
	bIsVerticalWallrun = CharacterMovementComponent->IsVerticalWallrunning();
	bIsLateralWallrun = CharacterMovementComponent->IsLateralWallrunning();
	
	PlayerRotation = UOvrlUtils::GetGravityRelativeRotation(PlayerCharacter->GetActorRotation(), CharacterMovementComponent->GetGravityDirection());
	bHasJustLanded = PlayerCharacter->bJustLanded;
	bHasDefaultAnimLayerClass = PlayerCharacter->HasDefaultAnimLayerClass();
}

void UOvrlPlayerAnimInstance::OnMontageInstanceStopped(FAnimMontageInstance& StoppedMontageInstance)
{
	Super::OnMontageInstanceStopped(StoppedMontageInstance);
	
	if (CharacterMovementComponent)
	{
		CharacterMovementComponent->ResetTraversal();
	}
}

bool UOvrlPlayerAnimInstance::IsPlayerAnimInstance() const
{
	return PlayerCharacter && PlayerCharacter->IsPlayerControlled();
}

FRotator UOvrlPlayerAnimInstance::GetSlideSlopeRotation() const
{
	if (!CharacterMovementComponent)
	{
		return FRotator::ZeroRotator;
	}
	
	const FVector PlayerRightVector = PlayerCharacter->GetActorRightVector();
	const FVector PlayerUpVector = PlayerCharacter->GetActorUpVector();
	const FVector GroundNormal = CharacterMovementComponent->GetGroundNormal();
	
	float OutSlopePitchAngle;
	float OutSlopeRollAngle;
	UKismetMathLibrary::GetSlopeDegreeAngles(PlayerRightVector, GroundNormal, PlayerUpVector, OutSlopePitchAngle, OutSlopeRollAngle);
	
	return FRotator(0.f, 0.f, -OutSlopePitchAngle);
}

bool UOvrlPlayerAnimInstance::IsIdle() const
{
	if (CharacterMovementComponent)
	{
		return CharacterMovementComponent->GetGait() == OvrlGaitTags::Idle && !bHasJustLanded && !bIsFalling;
	}
	
	return false;
}

bool UOvrlPlayerAnimInstance::ShouldTurnInPlace() const
{
	if (!CharacterMovementComponent)
	{
		return false;
	}
	
	FVector2D TargetThreshold = StandingRootYawThreshold;
	if (CharacterMovementComponent->Stance == OvrlStanceTags::Crouching)
	{
		TargetThreshold = CrouchingRootYawThreshold;
	}
	
	const bool bShouldTurnInPlace = RootYawOffset <= TargetThreshold.X || RootYawOffset >= TargetThreshold.Y;
	
	// We don't need to turn-in-place for FP mesh
	return !bIsFirstPersonABP && bIsPlayerValid && bShouldTurnInPlace;
}

FRotator UOvrlPlayerAnimInstance::GetWallrunCameraTiltRotation() const
{
	if (PlayerCharacter && CharacterMovementComponent)
	{
		// FRotator CameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
		FRotator CameraRotation = PlayerCharacter->GetControlRotation();
		CameraRotation = UOvrlUtils::GetGravityRelativeRotation(CameraRotation, CharacterMovementComponent->GetGravityDirection());
		return FRotator(CameraRotation.Roll, 0.f, 0.f);
	}
	
	return FRotator::ZeroRotator;
}

