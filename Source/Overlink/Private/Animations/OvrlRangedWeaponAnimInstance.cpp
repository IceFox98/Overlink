// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlRangedWeaponAnimInstance.h"

// Internal
#include "Weapons/OvrlRangedWeaponInstance.h"

void UOvrlRangedWeaponAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(EquippedWeapon))
	{
		WeaponRecoil = EquippedWeapon->GetWeaponKickbackRecoil();
		WeaponCameraRecoil = EquippedWeapon->GetWeaponCameraRecoil();
		WeaponAimTransform = EquippedWeapon->GetAimTransform();
	}
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
