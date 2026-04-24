// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OvrlProjectile.generated.h"

class UCapsuleComponent;
class UProjectileMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProjectileHit, const FHitResult&, HitResult);

UCLASS()
class AOvrlProjectile : public AActor
{
	GENERATED_BODY()

public:
	AOvrlProjectile();

protected:
	UFUNCTION(BlueprintCallable, Category = "Ovrl Projectile")
	void TriggerOnProjectileHit(const FHitResult& HitResult);

public:
	FOnProjectileHit OnProjectileHit;

protected:
	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
};
