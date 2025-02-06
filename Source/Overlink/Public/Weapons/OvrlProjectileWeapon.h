// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OBM_WeaponInstance.h"
#include "OBM_ProjectileWeapon.generated.h"

class USphereComponent;

/**
 *
 */
UCLASS()
class OVERLINK_API AOBM_ProjectileWeapon : public AOBM_WeaponInstance
{
	GENERATED_BODY()

public:

	AOBM_ProjectileWeapon();

public:

	virtual void Fire(const FHitResult& HitData) override;

	virtual void Reload() override;

	UFUNCTION(BlueprintCallable, Category = "Projectile Weapon")
		FTransform GetMuzzleTransform() const;

public:

	//// ----- COMPONENTS -----

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	TObjectPtr<USphereComponent> PickupSphere;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Weapon")
		TSubclassOf<UGameplayEffect> GE_ReloadDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Weapon")
		float ThrowForce;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Weapon")
		FName MuzzleSocketName;

protected:

	bool bIsReloading;
};
