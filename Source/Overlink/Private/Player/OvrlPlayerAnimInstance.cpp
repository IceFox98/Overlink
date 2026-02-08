// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"

// Engine
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemGlobals.h"
#include "OvrlUtils.h"

void UOvrlPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AOvrlPlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const UWorld* World = GetWorld();

	if (IsValid(World) && !World->IsGameWorld() && !IsValid(PlayerCharacter))
	{
		// Use default objects for editor preview.
		PlayerCharacter = GetMutableDefault<AOvrlPlayerCharacter>();
	}
#endif

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerCharacter))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
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

	const FRotator RotationOffset = UOvrlUtils::GetGravityRelativeRotation(FRotator(180.f, 0.f, 0.f), CharacterMovementComponent->GetGravityDirection());

	const FRotator ComposedRotator = UKismetMathLibrary::ComposeRotators(PlayerCharacter->GetControlRotation(), RotationOffset);
	//SpineRotation = UOvrlUtils::GetGravityRelativeRotation(FRotator(0.f, 0.f, ComposedRotator.Pitch), CharacterMovementComponent->GetGravityDirection());
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
}
