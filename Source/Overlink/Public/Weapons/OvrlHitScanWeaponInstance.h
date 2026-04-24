// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

#include "OvrlHitScanWeaponInstance.generated.h"

/**
 *
 */
UCLASS()
class OVERLINK_API AOvrlHitScanWeaponInstance : public AOvrlRangedWeaponInstance
{
	GENERATED_BODY()

protected:
	virtual void ProcessHit(const FHitResult& HitData) override;
	virtual void SpawnTrailVFX(const FHitResult& HitData);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Hit-Scan Weapon Instance", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNiagaraSystem> BulletTrailVFX;
};
