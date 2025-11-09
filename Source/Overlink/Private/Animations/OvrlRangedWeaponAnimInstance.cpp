// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlRangedWeaponAnimInstance.h"

// Internal
#include "Weapons/OvrlRangedWeaponInstance.h"

UOvrlRangedWeaponAnimInstance::UOvrlRangedWeaponAnimInstance()
{
	LookingSwayAlphaADS = .4f;
	MovementSwayAlphaADS = .5f;
	WalkSwayAlphaADS = .1f;
	JumpSwayAlphaADS = .15f;
}

void UOvrlRangedWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(EquippedWeapon))
	{
		AimSpeed = EquippedWeapon->GetAimSpeed();
		WeaponRecoilTranslation = EquippedWeapon->GetWeaponKickbackRecoil().GetLocation();
		WeaponRecoilRotation = EquippedWeapon->GetWeaponKickbackRecoil().GetRotation().Rotator();
		WeaponCameraRecoil = EquippedWeapon->GetWeaponCameraRecoil();
		WeaponAimTranslation = EquippedWeapon->GetAimTransform().GetLocation();
		WeaponAimRotation = EquippedWeapon->GetAimTransform().GetRotation().Rotator();
		LeftHandIKTransform = EquippedWeapon->GetLeftHandIKTransform();

		UpdateAim(DeltaTime);
	}
}

void UOvrlRangedWeaponAnimInstance::UpdateAim(float DeltaTime)
{
	const bool bIsWeaponAiming = EquippedWeapon->IsADS();
	const float TargetAimAlpha = bIsWeaponAiming ? 1.f : 0.f;
	AimAlpha = FMath::FInterpTo(AimAlpha, TargetAimAlpha, DeltaTime, AimSpeed);

	// Change aplha to recude the sway movement while player is aiming
	LookingSwayAlpha = bIsWeaponAiming ? LookingSwayAlphaADS : 1.f;
	MovementSwayAlpha = bIsWeaponAiming ? MovementSwayAlphaADS : 1.f;
	WalkSwayAlpha = bIsWeaponAiming ? WalkSwayAlphaADS : 1.f;
	JumpSwayAlpha = bIsWeaponAiming ? JumpSwayAlphaADS : 1.f;
}

void UOvrlRangedWeaponAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	Super::OnNewItemEquipped(NewEquippedItem);

	if (EquippedItem->IsA<AOvrlRangedWeaponInstance>())
	{
		EquippedWeapon = Cast<AOvrlRangedWeaponInstance>(EquippedItem);
	}
	else
	{
		EquippedWeapon = nullptr;
	}
}

bool UOvrlRangedWeaponAnimInstance::CheckCrouchLeaning()
{
	const bool bBaseCheck = Super::CheckCrouchLeaning();

	if (EquippedWeapon)
	{
		return bBaseCheck && !EquippedWeapon->IsADS();
	}

	return bBaseCheck;
}

float UOvrlRangedWeaponAnimInstance::CalculateCrouchLeanSpeed()
{
	const float BaseSpeed = Super::CalculateCrouchLeanSpeed();

	if (EquippedWeapon && EquippedWeapon->IsADS())
	{
		return BaseSpeed * 3.f;
	}

	return BaseSpeed;
}
