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

public:

	AOvrlHitScanWeaponInstance();

public:

	virtual void Fire(const FHitResult& HitData) override;

protected:

	virtual void SpawnFireVFX(const FHitResult& HitData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Hit-Scan Weapon Instance", meta = (AllowPrivateAccess = true))
		UNiagaraSystem* BulletTrailVFX;
};
