// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OBM_GameplayAbility_HitScanWeapon.h"

UOBM_GameplayAbility_HitScanWeapon::UOBM_GameplayAbility_HitScanWeapon()
{
	TraceMaxDistance = 25000.f;
}

void UOBM_GameplayAbility_HitScanWeapon::StartRangedWeaponTargeting()
{
	FHitResult HitResult;

	// TODO: Hit scan trace

	OnRangedWeaponTargetDataReady(HitResult);
}
