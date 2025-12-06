// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/Procedural/OvrlAnimAlphaModifiers.h"

#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Animations/OvrlRangedWeaponAnimInstance.h"

void UOvrlWeaponAimingAnimAlphaModifier::Initialize()
{
	RangedWeaponAnimInstance = GetTypedOuter<UOvrlRangedWeaponAnimInstance>();
}

void UOvrlWeaponAimingAnimAlphaModifier::ModifyAlpha(float& OutAlpha)
{
	if (RangedWeaponAnimInstance.IsValid())
	{
		if (AOvrlRangedWeaponInstance* RangedWeapon = RangedWeaponAnimInstance->GetEquippedWeapon())
		{
			if (RangedWeapon->IsADS())
			{
				OutAlpha *= AlphaMultiplier;
			}
		}
	}
}