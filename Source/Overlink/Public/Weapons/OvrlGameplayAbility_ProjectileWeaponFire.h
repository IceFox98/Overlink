// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlGameplayAbility_HitScanWeaponFire.h"

#include "OvrlGameplayAbility_ProjectileWeaponFire.generated.h"

class AOvrlProjectile;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlGameplayAbility_ProjectileWeaponFire : public UOvrlGameplayAbility_HitScanWeaponFire
{
	GENERATED_BODY()

public:

	UOvrlGameplayAbility_ProjectileWeaponFire();

protected:

	UFUNCTION(BlueprintCallable, Category = "Ovrl|Projectile Weapon Fire")
		void FireProjectile(const FHitResult& HitResult);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Projectile Weapon Fire")
		TSubclassOf<AOvrlProjectile> ProjectileClass;
};
