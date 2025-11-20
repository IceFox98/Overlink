// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlEquipmentAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Player/Components/OvrlCameraComponent.h"
#include "OvrlUtils.h"
#include "Overlink.h"

// Engine
#include "Curves/CurveVector.h"

UOvrlEquipmentAnimInstance::UOvrlEquipmentAnimInstance()
{
	MovementSwaySpeed = 20.f;
	MovementSwayMultiplier = 1.f;
	MovementSwayRollMultiplier = 2.f;
	WalkSwaySpeed = 20.f;

	LookingSwayAlpha = 1.f;
	MovementSwayAlpha = 1.f;
	WalkSwayAlpha = 1.f;
	JumpSwayAlpha = 1.f;

	LookingSwayRotationLimit = FVector2D::One();
	LookingSwayMovementMultiplier = FVector::One();
	LookingSwayStiffness = .5f;
	LookingSwayCriticalDampingFactor = .8f;
}

void UOvrlEquipmentAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
}

void UOvrlEquipmentAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(PlayerCharacter);
	PlayerCharacter->GetInventoryComponent()->OnItemEquipped.AddDynamic(this, &UOvrlEquipmentAnimInstance::OnNewItemEquipped);

	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();

	ensureAlways(WalkSwayTranslationCurve);
	ensureAlways(WalkSwayRotationCurve);
	ensureAlways(JumpSwayCurve);
}

void UOvrlEquipmentAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

}

void UOvrlEquipmentAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(PlayerCharacter))
	{
		UpdateLookingSway(DeltaTime);
		UpdateMovementSway(DeltaTime);
		UpdateWalkSway(DeltaTime);
		UpdateJumpSway(DeltaTime);
		UpdateLeftHandIKAplha(DeltaTime);
		UpdateCrouchLeanAlpha(DeltaTime);

		const FRotator CameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
		WallrunCameraTiltRotation = FRotator(CameraRotation.Roll, 0.f, 0.f);
	}
}

void UOvrlEquipmentAnimInstance::UpdateLookingSway(float DeltaTime)
{
	// Get camera delta movement
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(LastPlayerCameraRotation, PlayerCharacter->GetCameraComponent()->GetComponentRotation());

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -LookingSwayRotationLimit.Y, LookingSwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -LookingSwayRotationLimit.X, LookingSwayRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	//LastLookingSwayRotation = UKismetMathLibrary::QuaternionSpringInterp(FQuat(LastLookingSwayRotation), FQuat(TargetSwayRotation), SpringStateRotation, LookingSwayStiffness, LookingSwayCriticalDampingFactor, DeltaTime, .0006f, .3f).Rotator();

	// Speed = 3.5
	LastLookingSwayRotation = UKismetMathLibrary::RInterpTo(LastLookingSwayRotation, TargetSwayRotation, DeltaTime, 3.5f); 

	// Apply weapon sway looking to Anim BP
	LookingSwayTranslation = FVector(
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.X,
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.Z,
		LastLookingSwayRotation.Pitch * LookingSwayMovementMultiplier.Y
	);

	LookingSwayRotation = FRotator(
		LastLookingSwayRotation.Pitch * LookingSwayRotationMultiplier.Y,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.Z,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.X
	);




	//const FVector CameraDelta = FVector(SwayYaw, SwayPitch, 0.f);

	//LastLookingSwayTranslation = UKismetMathLibrary::VectorSpringInterp(LastLookingSwayTranslation, CameraDelta, SprintStateLookingSway, LookingSwayStiffness, LookingSwayCriticalDampingFactor, DeltaTime, 1.f, 0.3f);

	//// Apply weapon sway looking to Anim BP
	//LookingSwayTranslation = FVector(
	//	LastLookingSwayTranslation.X * LookingSwayMovementMultiplier.X,
	//	LastLookingSwayTranslation.X * LookingSwayMovementMultiplier.Z,
	//	LastLookingSwayTranslation.Y * LookingSwayMovementMultiplier.Y
	//);

	//LookingSwayRotation = FRotator(
	//	LastLookingSwayTranslation.Y * LookingSwayRotationMultiplier.Y,
	//	LastLookingSwayTranslation.X * LookingSwayRotationMultiplier.Z,
	//	LastLookingSwayTranslation.X * LookingSwayRotationMultiplier.X
	//);

	// Save the last sway rotation
	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
}

void UOvrlEquipmentAnimInstance::UpdateMovementSway(float DeltaTime)
{
	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is moving in any directions
	// If it's moving fully forward, the Dot result will be equals to the MaxWalkSpeed, so the division will return 1.
	const float ForwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorForwardVector()) / CharacterMovementComponent->MaxWalkSpeed;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const FVector TargetMovementAmount = FVector(ForwardAmount, RightwardAmount, 0.f) * MovementSwayMultiplier;

	MovementSwayTranslation = FMath::VInterpTo(MovementSwayTranslation, TargetMovementAmount, DeltaTime, MovementSwaySpeed);

	MovementSwayRotation = FRotator(0.f, 0.f, FMath::Clamp(MovementSwayTranslation.Y, -1.f, 1.f) * -MovementSwayRollMultiplier);
}

void UOvrlEquipmentAnimInstance::UpdateWalkSway(float DeltaTime)
{
	if (!WalkSwayTranslationCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "WalkSwayTraslationCurve is NULL!");
		return;
	}

	if (!WalkSwayRotationCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "WalkSwayRotationCurve is NULL!");
		return;
	}

	FVector TargetWalkSwayTranslation = FVector::ZeroVector;
	FRotator TargetWalkSwayRotation = FRotator::ZeroRotator;

	// Read the value only when player moves
	if (CharacterMovementComponent->GetLastInputVector().Length() > 0.f)
	{
		TargetWalkSwayTranslation = WalkSwayTranslationCurve->GetVectorValue(WalkSwayTime);
		const FVector RotationCurve = WalkSwayRotationCurve->GetVectorValue(WalkSwayTime);
		TargetWalkSwayRotation = FRotator(RotationCurve.Y, RotationCurve.Z, RotationCurve.X);
		WalkSwayTime += DeltaTime;
	}
	else
	{
		WalkSwayTime = 0.f;
	}

	// Simulate a walk animation
	WalkSwayTranslation = FMath::VInterpTo(WalkSwayTranslation, TargetWalkSwayTranslation, DeltaTime, WalkSwaySpeed);
	WalkSwayRotation = FMath::RInterpTo(WalkSwayRotation, TargetWalkSwayRotation, DeltaTime, WalkSwaySpeed);

	//// Calculate the rotation to apply when player walk
	//const FVector WalkRotationVector = WalkSwayTranslation * WalkSwayRotationMultiplier;
	//WalkSwayRotation = FRotator(WalkRotationVector.Y, WalkRotationVector.Z, WalkRotationVector.X);
}

void UOvrlEquipmentAnimInstance::UpdateJumpSway(float DeltaTime)
{
	if (!JumpSwayCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "JumpSwayCurve is NULL!");
		return;
	}

	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is jumping/falling
	// If the jump has just started, the Dot result will be equals to the JumpZVelocity, so the division will return 1.
	const float UpwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorUpVector()) / CharacterMovementComponent->JumpZVelocity;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const float SideSwayMultiplier = .4f;

	// Apply some side sway movement, only when player jumps sideway
	const FVector TargetJumpAmount = JumpSwayCurve->GetVectorValue(-UpwardAmount) * FVector(1.f, -RightwardAmount * SideSwayMultiplier, 1.f);

	JumpSwayTranslation = UKismetMathLibrary::VectorSpringInterp(JumpSwayTranslation, TargetJumpAmount, SpringStateJump, .5f, .45f, DeltaTime, 0.005f, 5.f);

	// Calculate the rotation to apply when player jumps sideway
	const FVector JumpRotationVector = FVector(JumpSwayTranslation.Y, 0.f, JumpSwayTranslation.Y) * -JumpSwayRotationMultiplier;
	JumpSwayRotation = FRotator(0.f, JumpRotationVector.Z, JumpRotationVector.X);
}

void UOvrlEquipmentAnimInstance::UpdateCrouchLeanAlpha(float DeltaTime)
{
	const bool bShouldLean = CheckCrouchLeaning(); // Call virtual function
	const float TargetCrouchLeanAlpha = bShouldLean ? 1.f : 0.f;
	const float CrouchLeanSpeed = CalculateCrouchLeanSpeed();
	CrouchLeanAlpha = FMath::FInterpTo(CrouchLeanAlpha, TargetCrouchLeanAlpha, DeltaTime, CrouchLeanSpeed);
}

void UOvrlEquipmentAnimInstance::UpdateLeftHandIKAplha(float DeltaTime)
{
	const bool bIsPlayerWallrunning = CharacterMovementComponent->IsWallrunning();
	const float TargetLeftHandIKAlpha = bIsPlayerWallrunning ? 0.f : 1.f;
	LeftHandIKAlpha = FMath::FInterpTo(LeftHandIKAlpha, TargetLeftHandIKAlpha, DeltaTime, 15.f);
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}

bool UOvrlEquipmentAnimInstance::CheckCrouchLeaning()
{
	return CharacterMovementComponent->IsCrouching();
}

float UOvrlEquipmentAnimInstance::CalculateCrouchLeanSpeed()
{
	return 10.f;
}
