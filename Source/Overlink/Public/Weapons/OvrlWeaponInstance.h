// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#include "OvrlWeaponInstance.generated.h"

DECLARE_DELEGATE_OneParam(FOnHitSomething, const FHitResult& HitData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFired, AOvrlWeaponInstance*, Weapon);

class USphereComponent;
class USoundBase;

USTRUCT()
struct FBulletImpactEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactDecal;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
};

/**
 *
 */
UCLASS()
class OVERLINK_API AOvrlWeaponInstance : public AOvrlEquipmentInstance
{
	GENERATED_BODY()

public:

	AOvrlWeaponInstance();

protected:

	virtual void BeginPlay() override;

public:

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void Fire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void StopFire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Weapon Instance", meta = (DisplayName = "On Fire"))
	void K2_OnFire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void StartReloading();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void PerformReload();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Weapon Instance")
	virtual bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	FTransform GetLeftHandIKTransform() const;

	void ToggleWeaponPhysics(bool bEnable);

protected:

	// @TODO: Should not be used here anymore
	UFUNCTION()
	void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	virtual void SpawnImpactVFX(const FHitResult& HitData);

private:

	void SpawnEffect(UNiagaraSystem* Effect, EPhysicalSurface SurfaceType, const FHitResult& HitData);

public:

	// ----- COMPONENTS -----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Components")
	TObjectPtr<USphereComponent> PickupSphere;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Weapon Instance")
	TSubclassOf<UGameplayEffect> GE_Damage;

	FOnHitSomething OnHitSomething;

	UPROPERTY(BlueprintAssignable)
	FOnFired OnFired;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Weapon Instance")
	TMap<TEnumAsByte<EPhysicalSurface>, FBulletImpactEffects> BulletImpactEffects;

	// Should be the skeletal mesh of the character holding the weapon
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> OwningSkeletalMesh;

private:

	bool bIsReloading;
};
