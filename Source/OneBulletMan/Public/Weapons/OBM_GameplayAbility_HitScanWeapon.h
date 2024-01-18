// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OBM_GameplayAbility_WeaponFire.h"
#include "OBM_GameplayAbility_HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API UOBM_GameplayAbility_HitScanWeapon : public UOBM_GameplayAbility_WeaponFire
{
	GENERATED_BODY()

public:

	UOBM_GameplayAbility_HitScanWeapon();

public:

	UFUNCTION(BlueprintCallable)
		void StartRangedWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent)
		void OnRangedWeaponTargetDataReady(const FHitResult& TargetData);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Fire", meta = (EditCondition = "bHitScanTrace"))
		float TraceMaxDistance;
};
