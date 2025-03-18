// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/OvrlEquipmentInstance.h"

#include "OvrlWeaponInstance.generated.h"

class USphereComponent;

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

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
		virtual void Fire(const FHitResult& HitData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Weapon Instance", meta = (DisplayName = "On Fire"))
		void K2_OnFire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Weapon Instance")
		virtual void Reload();

	void ToggleWeaponPhysics(bool bEnable);

	// @TODO: Should not be used here anymore
	UFUNCTION()
		void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:

	// ----- COMPONENTS -----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Components")
		TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Weapon Instance|Components")
		TObjectPtr<USphereComponent> PickupSphere;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Weapon Instance")
		TSubclassOf<UGameplayEffect> GE_Damage;
};
