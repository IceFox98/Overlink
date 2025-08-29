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

	ensureAlways(WalkSwayCurve);
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
		UpdateRunningAlpha(DeltaTime);

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

	LookingSwayRotation = UKismetMathLibrary::QuaternionSpringInterp(FQuat(LookingSwayRotation), FQuat(TargetSwayRotation), SpringStateRotation, .5f, .8f, DeltaTime, 0.006f).Rotator();

	// Apply weapon sway looking to Anim BP
	LookingSwayTranslation = FVector(0.f, LookingSwayRotation.Yaw, LookingSwayRotation.Pitch);

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
	if (!WalkSwayCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "WalkSwayCurve is NULL!");
		return;
	}

	FVector TargetSwayWalk = FVector::ZeroVector;

	// Read the value only when player moves
	if (CharacterMovementComponent->GetLastInputVector().Length() > 0.f)
	{
		TargetSwayWalk = WalkSwayCurve->GetVectorValue(GetWorld()->GetTimeSeconds());
	}

	// Simulate a walk animation
	WalkSwayTranslation = FMath::VInterpTo(WalkSwayTranslation, TargetSwayWalk, DeltaTime, WalkSwaySpeed);

	// Calculate the rotation to apply when player walk
	const FVector WalkRotationVector = WalkSwayTranslation * WalkSwayRotationMultiplier;
	WalkSwayRotation = FRotator(WalkRotationVector.Y, WalkRotationVector.Z, WalkRotationVector.X);
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

void UOvrlEquipmentAnimInstance::UpdateRunningAlpha(float DeltaTime)
{
	const bool bIsPlayerRunning = CharacterMovementComponent->IsRunning();
	const float TargetRunningAlpha = bIsPlayerRunning ? 1.f : 0.f;
	RunningAlpha = FMath::FInterpTo(RunningAlpha, TargetRunningAlpha, DeltaTime, RunningTransitionSpeed);
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}
