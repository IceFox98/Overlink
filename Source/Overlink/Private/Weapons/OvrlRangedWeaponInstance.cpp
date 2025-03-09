// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

AOvrlRangedWeaponInstance::AOvrlRangedWeaponInstance()
{
	BulletsPerCartridge = 1;
	MaxDamageRange = 25000.0f;
	FireRate = 400.f;
}

void AOvrlRangedWeaponInstance::AddSpread()
{

}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}
