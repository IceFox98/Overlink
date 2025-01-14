// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OBM_GameplayAbility_HitScanWeaponFire.h"

#include "OBM_GameplayAbility_ProjectileWeaponFire.generated.h"

class AOBM_Projectile;

/**
 *
 */
UCLASS()
class ONEBULLETMAN_API UOBM_GameplayAbility_ProjectileWeaponFire : public UOBM_GameplayAbility_HitScanWeaponFire
{
	GENERATED_BODY()

public:

	UOBM_GameplayAbility_ProjectileWeaponFire();

protected:

	UFUNCTION(BlueprintCallable, Category = "OBM|Projectile Weapon Fire")
		void FireProjectile(const FHitResult& HitResult);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Projectile Weapon Fire")
		TSubclassOf<AOBM_Projectile> ProjectileClass;
};
