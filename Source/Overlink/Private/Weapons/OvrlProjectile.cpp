// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapons/OvrlProjectile.h"

// Engine
#include "GameFramework/ProjectileMovementComponent.h"

AOvrlProjectile::AOvrlProjectile()
{
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 11000.f;
	ProjectileMovement->MaxSpeed = 11000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bSweepCollision = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AOvrlProjectile::TriggerOnProjectileHit(const FHitResult& HitResult)
{
	OnProjectileHit.Broadcast(HitResult);

	Destroy();
}
