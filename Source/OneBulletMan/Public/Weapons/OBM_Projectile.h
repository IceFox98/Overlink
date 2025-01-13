// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OBM_Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS(config = Game)
class AOBM_Projectile : public AActor
{
	GENERATED_BODY()


public:

	AOBM_Projectile();

protected:
	virtual void BeginPlay() override;

public:


	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetDamage(const TSubclassOf<UGameplayEffect>& DamageClass) { GE_Damage = DamageClass; };

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

private:

	UPROPERTY()
		TSubclassOf<UGameplayEffect> GE_Damage;
};

