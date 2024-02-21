// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment/EquipmentInstance.h"
#include "WeaponInstance.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API AWeaponInstance : public AEquipmentInstance
{
	GENERATED_BODY()

public:

	AWeaponInstance();
	
public:

	virtual void Tick(float DeltaSeconds) override;

public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Fire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Reload();

public:

	// ----- COMPONENTS -----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;

public:
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	//	UAnimMontage* WeaponEquipMontage;

private:

	double TimeLastFired = 0.0;

};
