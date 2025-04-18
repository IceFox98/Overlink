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
	SwayMovementSpeed = 20.f;
	SwayMovementMultiplier = 1.f;
	SwayMovementRollMultiplier = 2.f;
	SwayWalkSpeed = 20.f;
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
		UpdateSwayMovement(DeltaTime);
		UpdateSwayLooking(DeltaTime);
		UpdateSwayWalk(DeltaTime);
		UpdateSwayJump(DeltaTime);
	}
}

void UOvrlEquipmentAnimInstance::UpdateSwayLooking(float DeltaTime)
{
	// Get camera delta movement
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(LastPlayerCameraRotation, PlayerCharacter->GetCameraComponent()->GetComponentRotation());

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -SwayLookingRotationLimit.Y, SwayLookingRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -SwayLookingRotationLimit.X, SwayLookingRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	SwayRotationAmount = UKismetMathLibrary::QuaternionSpringInterp(FQuat(SwayRotationAmount), FQuat(TargetSwayRotation), SpringStateRotation, .5f, .8f, DeltaTime, 0.006f).Rotator();

	// Apply weapon sway looking to Anim BP
	SwayLookingAmount = FVector(0.f, SwayRotationAmount.Yaw, SwayRotationAmount.Pitch);

	// Save the last sway rotation
	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
}

void UOvrlEquipmentAnimInstance::UpdateSwayMovement(float DeltaTime)
{
	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is moving in any directions
	// If it's moving fully forward, the Dot result will be equals to the MaxWalkSpeed, so the division will return 1.
	const float ForwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorForwardVector()) / CharacterMovementComponent->MaxWalkSpeed;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const FVector TargetMovementAmount = FVector(ForwardAmount, RightwardAmount, 0.f) * SwayMovementMultiplier;

	SwayMovementAmount = FMath::VInterpTo(SwayMovementAmount, TargetMovementAmount, DeltaTime, SwayMovementSpeed);

	SwayMovementRotationAmount = FRotator(0.f, 0.f, FMath::Clamp(SwayMovementAmount.Y, -1.f, 1.f) * -SwayMovementRollMultiplier);
}

void UOvrlEquipmentAnimInstance::UpdateSwayWalk(float DeltaTime)
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
	SwayWalkAmount = FMath::VInterpTo(SwayWalkAmount, TargetSwayWalk, DeltaTime, SwayWalkSpeed);

	// Calculate the rotation to apply when player walk
	const FVector WalkRotationVector = SwayWalkAmount * SwayWalkRotationMultiplier;
	SwayWalkRotationAmount = FRotator(WalkRotationVector.Y, WalkRotationVector.Z, WalkRotationVector.X);
}

void UOvrlEquipmentAnimInstance::UpdateSwayJump(float DeltaTime)
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

	SwayJumpAmount = UKismetMathLibrary::VectorSpringInterp(SwayJumpAmount, TargetJumpAmount, SpringStateJump, .5f, .45f, DeltaTime, 0.005f, 5.f);

	// Calculate the rotation to apply when player jumps sideway
	const FVector JumpRotationVector = FVector(SwayJumpAmount.Y, 0.f, SwayJumpAmount.Y) * SwayJumpRotationMultiplier;
	SwayJumpRotationAmount = FRotator(0.f, JumpRotationVector.Z, JumpRotationVector.X);
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}
