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

	UFUNCTION(BlueprintCallable)
		void UpdateFiringTime();

public:

	// ---- COMPONENTS ----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USphereComponent* SphereCollision;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimMontage* WeaponEquipMontage;

private:

	bool bHitSomething;

	double TimeLastFired = 0.0;

};
