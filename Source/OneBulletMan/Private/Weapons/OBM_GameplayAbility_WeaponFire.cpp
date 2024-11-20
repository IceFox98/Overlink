// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OBM_GameplayAbility_WeaponFire.h"
#include "Weapons/ProjectileWeapon.h"
#include "Weapons/Projectile.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UOBM_GameplayAbility_WeaponFire::UOBM_GameplayAbility_WeaponFire()
{
}

void UOBM_GameplayAbility_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	if (PC)
	{
		if (AProjectileWeapon* ProjectileWeapon = Cast<AProjectileWeapon>(GetCurrentSourceObject()))
		{
			const FVector HitTraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector HitTraceEnd = HitTraceStart + PC->PlayerCameraManager->GetActorForwardVector() * 20000.f;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(ProjectileWeapon);
			Params.AddIgnoredActor(ProjectileWeapon->GetOwner());

			FHitResult Hit;
			GetWorld()->LineTraceSingleByChannel(Hit, HitTraceStart, HitTraceEnd, ECC_Visibility, Params);

			const FVector MuzzleLocation = ProjectileWeapon->GetMuzzleTransform().GetLocation();

			FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();

			if (Hit.bBlockingHit)
			{
				// Get rotation of the vector that start from Muzzle Location to Impact Point
				//SpawnRotation = FRotationMatrix::MakeFromX(Hit.ImpactPoint - MuzzleLocation).Rotator();

				SpawnRotation = (Hit.ImpactPoint - MuzzleLocation).Rotation();
			}

			const FTransform SpawnTransform(SpawnRotation, MuzzleLocation);

			AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, SpawnTransform, ProjectileWeapon->GetOwner(), ProjectileWeapon->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Projectile)
			{
				Projectile->SetDamage(GE_Damage);
			}

			UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

			ProjectileWeapon->Fire(); // Best time to implement VFX/Sounds/...
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
