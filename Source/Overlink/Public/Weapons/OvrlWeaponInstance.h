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

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire(const FHitResult& HitData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon", meta = (DisplayName = "On Fire"))
		void K2_OnFire(const FHitResult& HitData);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Reload();

	void ToggleWeaponPhysics(bool bEnable);

	UFUNCTION()
		void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:

	// ----- COMPONENTS -----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USphereComponent> PickupSphere;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	//	UAnimMontage* WeaponEquipMontage;

private:

	double TimeLastFired = 0.0;

};
