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

public:

	AOvrlProjectileWeaponInstance();

public:

	virtual void Fire(const FHitResult& HitData) override;

	virtual void StartReloading() override;

	UFUNCTION(BlueprintCallable, Category = "Ovrl|Projectile Weapon Fire")
		void FireProjectile(const FHitResult& HitResult);

public:

	//// ----- COMPONENTS -----

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	TObjectPtr<USphereComponent> PickupSphere;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Projectile Weapon Fire")
		TSubclassOf<AOvrlProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Weapon")
		TSubclassOf<UGameplayEffect> GE_ReloadDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Weapon")
		float ThrowForce;

protected:

	bool bIsReloading;
};
