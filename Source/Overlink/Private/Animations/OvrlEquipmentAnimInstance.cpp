// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlEquipmentAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Player/Components/OvrlCameraComponent.h"

// Engine
#include "Curves/CurveVector.h"

void UOvrlEquipmentAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	SwayMovementSpeed = 20.f;
}

void UOvrlEquipmentAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(PlayerCharacter);
	PlayerCharacter->GetInventoryComponent()->OnItemEquipped.AddDynamic(this, &UOvrlEquipmentAnimInstance::OnNewItemEquipped);

	check(JumpSwayCurve);
}

void UOvrlEquipmentAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

}

void UOvrlEquipmentAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(EquippedItem))
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

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -SwayRotationLimit.Y, SwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -SwayRotationLimit.X, SwayRotationLimit.X);
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

	const FVector TargetMovementAmount = FVector(ForwardAmount, RightwardAmount, 0.f);

	SwayMovementAmount = FMath::VInterpTo(SwayMovementAmount, TargetMovementAmount, DeltaTime, SwayMovementSpeed);
}

void UOvrlEquipmentAnimInstance::UpdateSwayWalk(float DeltaTime)
{
	FVector TargetSwayWalk = FVector::ZeroVector;

	if (CharacterMovementComponent->GetLastInputVector().Length() > 0.f)
	{
		TargetSwayWalk = EquippedItem->GetWalkSwayCurve()->GetVectorValue(GetWorld()->GetTimeSeconds());
	}

	SwayWalkAmount = FMath::VInterpTo(SwayWalkAmount, TargetSwayWalk, DeltaTime, SwayWalkSpeed);
}

void UOvrlEquipmentAnimInstance::UpdateSwayJump(float DeltaTime)
{
	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is jumping/falling
	// If the jump has just started, the Dot result will be equals to the JumpZVelocity, so the division will return 1.
	const float UpwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorUpVector()) / CharacterMovementComponent->JumpZVelocity;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const float SideSwayMultiplier = .4f;

	// Apply some side sway movement if jumping while moving right/left
	const FVector TargetJumpAmount = JumpSwayCurve->GetVectorValue(-UpwardAmount) * FVector(1.f, -RightwardAmount * SideSwayMultiplier, 1.f);

	SwayJumpAmount = UKismetMathLibrary::VectorSpringInterp(SwayJumpAmount, TargetJumpAmount, SpringStateJump, .5f, .45f, DeltaTime, 0.005f, 5.f);
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}
