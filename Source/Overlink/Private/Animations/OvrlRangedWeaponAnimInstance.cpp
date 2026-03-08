// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlRangedWeaponAnimInstance.h"

// Internal
#include "Weapons/OvrlRangedWeaponInstance.h"

UOvrlRangedWeaponAnimInstance::UOvrlRangedWeaponAnimInstance()
{
	LookingSwayAlphaADS = .4f;
	JumpSwayAlphaADS = .15f;
}

void UOvrlRangedWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(EquippedWeapon))
	{
		AimTime = EquippedWeapon->GetAimTime();

		WeaponRecoilTranslation = EquippedWeapon->GetWeaponKickbackRecoil().GetLocation();
		WeaponRecoilRotation = EquippedWeapon->GetWeaponKickbackRecoil().GetRotation().Rotator();
		
		WeaponAimTranslation = EquippedWeapon->GetAimTransform().GetLocation();
		WeaponAimRotation = EquippedWeapon->GetAimTransform().GetRotation().Rotator();
		
		UpdateAim(DeltaTime);

		InitialTransformAlpha = 1.f - AimAlpha;
	}
}

void UOvrlRangedWeaponAnimInstance::UpdateAim(float DeltaTime)
{
	const bool bIsWeaponAiming = EquippedWeapon->IsADS();
	const float TargetAimAlpha = bIsWeaponAiming ? 1.f : 0.f;

	if (bIsWeaponAiming)
	{
		LerpAimAlpha = FMath::Min(LerpAimAlpha + DeltaTime / AimTime, TargetAimAlpha);
	}
	else
	{
		LerpAimAlpha = FMath::Max(LerpAimAlpha - DeltaTime / AimTime, TargetAimAlpha);
	}

	AimAlpha = FMath::InterpEaseIn(0.f, 1.f, LerpAimAlpha, 1.5f);

	// Change aplha to recude the sway movement while player is aiming
	LookingSwayAlpha = bIsWeaponAiming ? LookingSwayAlphaADS : 1.f;
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
