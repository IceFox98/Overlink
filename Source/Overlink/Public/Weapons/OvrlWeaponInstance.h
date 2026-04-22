// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "NiagaraFunctionLibrary.h"

#include "OvrlWeaponInstance.generated.h"

DECLARE_DELEGATE_OneParam(FOnHitSomething, const FHitResult& HitData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFired, AOvrlWeaponInstance*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReloaded, AOvrlWeaponInstance*, Weapon);

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

public:
	virtual void OnEquipped_Implementation() override;
	virtual void OnUnequipped_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void Fire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void StopFire();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Weapon Instance", meta = (DisplayName = "On Fire"))
	void K2_OnFire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void StartReloading();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
	virtual void EndReloading();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Weapon Instance")
	virtual bool IsReloading() const { return bIsReloading; }

	virtual FTransform GetLeftHandIKTransform() const override;

protected:
	virtual void SpawnImpactVFX(const FHitResult& HitData);
	virtual float ComputeDamage(const FHitResult& HitData) const;

private:
	void SpawnEffect(UNiagaraSystem* Effect, EPhysicalSurface SurfaceType, const FHitResult& HitData);

public:
	// ----- COMPONENTS -----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Components")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

public:
	FOnHitSomething OnHitSomething;

	UPROPERTY(BlueprintAssignable)
	FOnFired OnFired;

	UPROPERTY(BlueprintAssignable)
	FOnReloaded OnReloaded;

protected:
	// Used for weapon IK. It's the name of the owner skeletal mesh IK bone to which this weapon is attached.
	// You should not edit this unless you changed something in the owner skeleton hierarchy.
	// NOTE: Don't use the socket name used to attach the actor, since it has different transform than hand.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance")
	FName OwnerAttachBoneName = TEXT("hand_r");

	// Name of the weapon's skeletal mesh socket used during IK.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance")
	FName LeftHandIKSocketName = TEXT("LeftHandIK");

	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Weapon Instance")
	TMap<TEnumAsByte<EPhysicalSurface>, FBulletImpactEffects> BulletImpactEffects;
	
	// Damage that will be set to the Gameplay Effect.
	// This value will be subtracted to the target Health.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Damage")
	float BaseDamage = 1.f;

	// Gameplay Effect that will be applied to the target hit by this weapon.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Damage")
	TSubclassOf<UGameplayEffect> GE_Damage;
	
	// Tag used to set the damage magnitude in the Gameplay Effect.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Damage")
	FGameplayTag DamageMagnitudeTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Damage")
	TMap<TEnumAsByte<EPhysicalSurface>, float> DamageSurfaceMultipliers;

private:
	bool bIsReloading = false;
};
