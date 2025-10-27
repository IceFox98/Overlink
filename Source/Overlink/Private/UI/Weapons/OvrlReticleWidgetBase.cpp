// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/OvrlReticleWidgetBase.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

void UOvrlReticleWidgetBase::InitializeFromWeapon(AOvrlRangedWeaponInstance* Weapon)
{
	check(Weapon);
	WeaponInstance = Weapon;
}

