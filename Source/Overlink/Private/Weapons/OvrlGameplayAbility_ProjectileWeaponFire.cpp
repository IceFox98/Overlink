// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OBM_GameplayAbility_ProjectileWeaponFire.h"
#include "Weapons/OBM_ProjectileWeapon.h"
#include "Weapons/OBM_Projectile.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UOBM_GameplayAbility_ProjectileWeaponFire::UOBM_GameplayAbility_ProjectileWeaponFire()
{
	TraceMaxDistance = 25000.f;
}

void UOBM_GameplayAbility_ProjectileWeaponFire::FireProjectile(const FHitResult& HitResult)
{
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0))
	{
		if (AOBM_WeaponInstance* WeaponInstance = Cast<AOBM_WeaponInstance>(GetCurrentSourceObject()))
		{
			if (const AOBM_ProjectileWeapon* ProjectileWeapon = Cast<AOBM_ProjectileWeapon>(WeaponInstance))
			{
				const FVector MuzzleLocation = ProjectileWeapon->GetMuzzleTransform().GetLocation();

				FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();

				if (HitResult.bBlockingHit)
				{
					// Get rotation of the vector that start from Muzzle Location to Impact Point
					SpawnRotation = (HitResult.ImpactPoint - MuzzleLocation).Rotation();
				}

				const FTransform SpawnTransform(SpawnRotation, MuzzleLocation);

				AOBM_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AOBM_Projectile>(ProjectileClass, SpawnTransform, ProjectileWeapon->GetOwner(), ProjectileWeapon->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (Projectile)
				{
					Projectile->SetDamage(GE_Damage);
				}

				UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

				WeaponInstance->Fire(HitResult);
			}
		}
	}
}