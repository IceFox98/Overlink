// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OBM_GameplayAbility_WeaponFire.h"
#include "Weapons/OBM_ProjectileWeapon.h"
#include "Weapons/OBM_Projectile.h"

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
		if (AOBM_WeaponInstance* WeaponInstance = Cast<AOBM_WeaponInstance>(GetCurrentSourceObject()))
		{
			const FVector HitTraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector HitTraceEnd = HitTraceStart + PC->PlayerCameraManager->GetActorForwardVector() * 20000.f;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(WeaponInstance);
			Params.AddIgnoredActor(WeaponInstance->GetOwner());

			FHitResult Hit;
			GetWorld()->LineTraceSingleByChannel(Hit, HitTraceStart, HitTraceEnd, ECC_Visibility, Params);

			if (AOBM_ProjectileWeapon* ProjectileWeapon = Cast<AOBM_ProjectileWeapon>(WeaponInstance))
			{
				const FVector MuzzleLocation = ProjectileWeapon->GetMuzzleTransform().GetLocation();

				FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();

				if (Hit.bBlockingHit)
				{
					// Get rotation of the vector that start from Muzzle Location to Impact Point
					//SpawnRotation = FRotationMatrix::MakeFromX(Hit.ImpactPoint - MuzzleLocation).Rotator();

					SpawnRotation = (Hit.ImpactPoint - MuzzleLocation).Rotation();
				}

				const FTransform SpawnTransform(SpawnRotation, MuzzleLocation);

				AOBM_Projectile* Projectile = GetWorld()->SpawnActorDeferred<AOBM_Projectile>(ProjectileClass, SpawnTransform, ProjectileWeapon->GetOwner(), ProjectileWeapon->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

				if (Projectile)
				{
					Projectile->SetDamage(GE_Damage);
				}

				UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

			}

			WeaponInstance->Fire(Hit); // Best time to implement VFX/Sounds/...
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
