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
			const FVector SpawnLocation = ProjectileWeapon->GetMuzzleTransform().GetLocation();
			const FRotator SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();
			const FTransform SpawnTransform(SpawnRotation, SpawnLocation);

			AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, SpawnTransform, ProjectileWeapon->GetOwner(), ProjectileWeapon->GetInstigator(), ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

			if (Projectile)
			{
				Projectile->SetDamage(GE_Damage);
			}

			UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);

			// Best time to implement VFX/Sounds/...
			ProjectileWeapon->Fire();
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
