// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/OvrlProjectileWeaponInstance.h"
#include "Weapons/OvrlProjectile.h"
#include "OvrlLogUtils.h"

// Engine
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

void AOvrlProjectileWeaponInstance::ProcessHit(const FHitResult& HitData)
{
	// Don't call Super() to avoid call OnHitSomething event.
	// The projectile will notify the hit.
	
	if (!ProjectileClass)
	{
		OVRL_LOG_WARN(LogOverlink, true, "ProjectileClass is not set!");
		return;
	}

	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		const FVector MuzzleLocation = GetMuzzleTransform().GetLocation();

		FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();

		if (HitData.bBlockingHit)
		{
			// Get rotation of the vector that start from Muzzle Location to Impact Point
			SpawnRotation = (HitData.ImpactPoint - MuzzleLocation).Rotation();
		}

		const FTransform SpawnTransform(SpawnRotation, MuzzleLocation);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		
		AOvrlProjectile* Projectile = GetWorld()->SpawnActor<AOvrlProjectile>(ProjectileClass, SpawnTransform, SpawnParams);
		
		if (Projectile)
		{
			Projectile->OnProjectileHit.AddDynamic(this, &AOvrlProjectileWeaponInstance::OnProjectileHit);
		}
	}
}

void AOvrlProjectileWeaponInstance::OnProjectileHit(const FHitResult& Hit)
{
	DealDamageToTargetFromHit(Hit);
	SpawnImpactVFX(Hit);
}
