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
}

void UOvrlEquipmentAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(PlayerCharacter);
	PlayerCharacter->GetInventoryComponent()->OnItemEquipped.AddDynamic(this, &UOvrlEquipmentAnimInstance::OnNewItemEquipped);
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

		// Sway applied when moving the camera around (mouse movement)
		UpdateSwayLooking(DeltaTime);
	}
}

void UOvrlEquipmentAnimInstance::UpdateSwayLooking(float DeltaTime)
{
	// Get camera delta movement
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(LastPlayerCameraRotation, PlayerCharacter->GetCameraComponent()->GetComponentRotation());

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -EquippedItem->SwayRotationLimit.Y, EquippedItem->SwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -EquippedItem->SwayRotationLimit.X, EquippedItem->SwayRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	SwayRotationAmount = UKismetMathLibrary::QuaternionSpringInterp(FQuat(SwayRotationAmount), FQuat(TargetSwayRotation), SpringStateRotation, .5f, .8f, DeltaTime, 0.006f).Rotator();
	//WeaponSwayRotation = FMath::RInterpTo(WeaponSwayRotation, TargetSwayRotation, DeltaTime, EquippedWeapon->WeaponSwayRotationSpeed);

	// Apply weapon sway looking to Anim BP
	SwayLookingAmount = FVector(0.f, SwayRotationAmount.Yaw, SwayRotationAmount.Pitch);

	// Save the last sway rotation
	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
}

void UOvrlEquipmentAnimInstance::UpdateSwayMovement(float DeltaTime)
{
	// Manage sway movement
	const FVector DeltaSwayMovement = CharacterMovementComponent->GetRelativeLastUpdateVelocity();

	const float SwayX = FMath::Clamp(DeltaSwayMovement.X, -EquippedItem->SwayMovementLimit.X, EquippedItem->SwayMovementLimit.X);
	const float SwayY = FMath::Clamp(DeltaSwayMovement.Y, -EquippedItem->SwayMovementLimit.Y, EquippedItem->SwayMovementLimit.Y);
	const float SwayZ = FMath::Clamp(DeltaSwayMovement.Z, -1.f, EquippedItem->SwayMovementLimit.Z);

	FVector TargetSwayMovement = FVector::ZeroVector;

	if (CharacterMovementComponent->GetLastInputVector().Length() > 0.f)
	{
		TargetSwayMovement = EquippedItem->GetWalkSwayCurve()->GetVectorValue(GetWorld()->GetTimeSeconds());
	}

	SwayMovementAmount = FMath::VInterpTo(SwayMovementAmount, TargetSwayMovement, DeltaTime, EquippedItem->SwayMovementSpeed);
	//WeaponSwayMovement = UKismetMathLibrary::VectorSpringInterp(WeaponSwayMovement, TargetSwayMovement, SpringStateMovement, .4f, .3, DeltaTime, 0.006f);

	//OVRL_LOG("Delta: %s - Current: %s - Target: %s", *DeltaSwayMovement.ToString(), *WeaponSwayMovement.ToString(), *TargetSwayMovement.ToString());

	//WeaponSwayMovementPrev = CharacterMovementComponent->GetLastUpdateVelocity();
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}
