// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OBM_EnemyBase.h"
#include "OBM_Turret.generated.h"

class UCapsuleComponent;

UCLASS()
class ONEBULLETMAN_API AOBM_Turret : public AOBM_EnemyBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOBM_Turret();


public:

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "Turret")
		void SetTarget(APawn* InTargetPawn);

	UFUNCTION()
		void Fire();

public:

	// ---- COMPONENTS ----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<UCapsuleComponent> Capsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<UStaticMeshComponent> BaseMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<UStaticMeshComponent> CannonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<USceneComponent> Muzzle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turret|Components")
		TObjectPtr<UStaticMeshComponent> Laser;

public:

	UPROPERTY(EditAnywhere, Category = "Turret|Combat")
		TSubclassOf<AActor> ProjectileClass;

	// Amounts of bullets shot per second.
	UPROPERTY(EditAnywhere, Category = "Turret|Combat")
		float RPS;

	UPROPERTY(EditAnywhere, Category = "Turret|Combat")
		float FireInitialDelay;

	UPROPERTY(BlueprintReadOnly, Category = "Turret|Combat")
		float FireRate;

	// The higher the value is, the less time it takes for the turret to lock on the target.
	UPROPERTY(EditAnywhere, Category = "Turret|Combat")
		float SnapRotationSpeed;

	UPROPERTY(EditAnywhere, Category = "Turret|Laser")
		FVector LaserEndPointOffset;

protected:

	UPROPERTY()
		APawn* TargetPawn;

	FTimerHandle TimerHandle_Fire;

	float InterpSpeed;
};
