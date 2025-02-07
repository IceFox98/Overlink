// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlGameplayAbility_ProjectileWeaponFire.h"
#include "Weapons/OvrlProjectileWeapon.h"
#include "Weapons/OvrlProjectile.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UOvrlGameplayAbility_ProjectileWeaponFire::UOvrlGameplayAbility_ProjectileWeaponFire()
{
	TraceMaxDistance = 25000.f;
}

void UOvrlGameplayAbility_ProjectileWeaponFire::FireProjectile(const FHitResult& HitResult)
{
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0))
	{
		if (AOvrlWeaponInstance* WeaponInstance = Cast<AOvrlWeaponInstance>(GetCurrentSourceObject()))
		{
			if (const AOvrlProjectileWeapon* ProjectileWeapon = Cast<AOvrlProjectileWeapon>(WeaponInstance))
			{
				const FVector MuzzleLocation = ProjectileWeapon->GetMuzzleTransform().GetLocation();

				FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();

				if (HitResult.bBlockingHit)
				{
					// Get rotation of the vector that start from Muzzle Location to Impact Point
					SpawnRotation = (HitResult.ImpactPoint - MuzzleLocation).Rotation();
				}

				const FTransform SpawnTransform(SpawnRotation, MuzzleLocation);

				AOvrlProjectile* Projectile = GetWorld()->SpawnActorDeferred<AOvrlProjectile>(ProjectileClass, SpawnTransform, ProjectileWeapon->GetOwner(), ProjectileWeapon->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

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