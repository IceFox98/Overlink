// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlProjectileWeapon.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include "OvrlUtils.h"

AOvrlProjectileWeapon::AOvrlProjectileWeapon()
{
	//PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	//PickupSphere->SetupAttachment(RootComponent);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThrowForce = 3200.f;
	MuzzleSocketName = "Muzzle";
}

void AOvrlProjectileWeapon::Fire(const FHitResult& HitData)
{
	if (bIsReloading)
		return;

	Super::Fire(HitData);
}

void AOvrlProjectileWeapon::Reload()
{
	Super::Reload();

	if (!Owner)
	{
		OVRL_LOG_ERR(LogTemp, true, "Owner is NULL!");
		return;
	}

	if (!bIsReloading) // Throw the weapon
	{
		bIsReloading = true;

		// Detach weapon from player
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		ToggleWeaponPhysics(true);

		//WeaponMesh->SetNotifyRigidBodyCollision(true);
		//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//WeaponMesh->SetSimulatePhysics(true);

		FVector OutLocation;
		FRotator OutRotation;
		Owner->GetActorEyesViewPoint(OutLocation, OutRotation);

		// Throw weapon
		const FVector ThrowVelocity = OutRotation.Vector() * ThrowForce;
		WeaponMesh->SetAllPhysicsLinearVelocity(ThrowVelocity);
	}
	else
	{
		if (GE_ReloadDamage)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(this);
			QueryParams.AddIgnoredActor(Owner);

			// Search for any pawn between the weapon and the player
			TArray<FHitResult> HitResults;
			GetWorld()->LineTraceMultiByObjectType(HitResults, GetActorLocation(), Owner->GetActorLocation(), ECC_Pawn, QueryParams);

			DrawDebugLine(GetWorld(), GetActorLocation(), Owner->GetActorLocation(), FColor::Red, false, 2.f);

			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator()))
			{
				for (const FHitResult& HitResult : HitResults)
				{
					if (UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitResult.GetActor()))
					{
						// Apply damage to each pawn
						ASC->ApplyGameplayEffectToTarget(GE_ReloadDamage->GetDefaultObject<UGameplayEffect>(), TargetASC, 1.f, ASC->MakeEffectContext());
					}
				}
			}
		}
		else
		{
			OVRL_LOG_ERR(LogTemp, true, "GE_ReloadDamage is NULL!");
		}

		// TODO: Play VFX

		// Reset physics
		ToggleWeaponPhysics(false);

		//WeaponMesh->SetAllPhysicsLinearVelocity(FVector::Zero());
		//WeaponMesh->SetNotifyRigidBodyCollision(false);
		//WeaponMesh->SetSimulatePhysics(false);
		//WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OnEquipped();

		bIsReloading = false;
	}
}

FTransform AOvrlProjectileWeapon::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}
