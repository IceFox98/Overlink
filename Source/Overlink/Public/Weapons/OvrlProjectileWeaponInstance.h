// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

#include "OvrlProjectileWeaponInstance.generated.h"

class AOvrlProjectile;

/**
 *
 */
UCLASS()
class OVERLINK_API AOvrlProjectileWeaponInstance : public AOvrlRangedWeaponInstance
{
	GENERATED_BODY()

protected:
	virtual void ProcessHit(const FHitResult& HitData) override;
	
	UFUNCTION()
	void OnProjectileHit(const FHitResult& HitResult);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Projectile Weapon Instance")
	TSubclassOf<AOvrlProjectile> ProjectileClass;
};
