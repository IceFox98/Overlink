// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/OvrlWeaponInstance.h"

#include "Core/Interfaces/OvrlDamageable.h"
#include "OvrlGameplayTags.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

AOvrlWeaponInstance::AOvrlWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	DamageMagnitudeTag = OvrlDataTags::Damage;
	DamageSurfaceMultipliers.Add(EPhysicalSurface::SurfaceType4, 2.f); // Weak Spot
}

void AOvrlWeaponInstance::OnEquipped_Implementation()
{
	Super::OnEquipped_Implementation();

	WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	if (ACharacter* OwningPawn = Cast<ACharacter>(GetOwner()))
	{
		// Get First Person skeletal mesh
		OwnerSkeletalMesh = OwningPawn->GetMesh();
	}
}

void AOvrlWeaponInstance::OnUnequipped_Implementation()
{
	Super::OnUnequipped_Implementation();

	WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::None;
}

void AOvrlWeaponInstance::Fire(const FHitResult& HitData)
{
	K2_OnFire(HitData);

	if (HitData.GetActor() && HitData.GetActor()->Implements<UOvrlDamageable>())
	{
		OnHitSomething.ExecuteIfBound(HitData);
	}

	OnFired.Broadcast(this);
}

void AOvrlWeaponInstance::StopFire()
{
	// No implementation
}

void AOvrlWeaponInstance::StartReloading()
{
	bIsReloading = true;
}

void AOvrlWeaponInstance::EndReloading()
{
	bIsReloading = false;

	OnReloaded.Broadcast(this);
}

float AOvrlWeaponInstance::ComputeDamage(const FHitResult& HitData) const
{
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;

	if (HitData.PhysMaterial.IsValid())
	{
		SurfaceType = HitData.PhysMaterial->SurfaceType;
	}

	// Get damage multiplier based on surface type hit.
	// If surface type is not in the list, set multiplier to 1 as default.
	const float DamageMultiplier = DamageSurfaceMultipliers.FindRef(SurfaceType, 1.f);
	return BaseDamage * DamageMultiplier;
}

FTransform AOvrlWeaponInstance::GetLeftHandIKTransform() const
{
	if (ensure(WeaponMesh && OwnerSkeletalMesh))
	{
		const FTransform SocketTransform = WeaponMesh->GetSocketTransform(LeftHandIKSocketName);
		FVector OutPosition;
		FRotator OutRotation;
		OwnerSkeletalMesh->TransformToBoneSpace(OwnerAttachBoneName, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), OutPosition, OutRotation);

		return { OutRotation, OutPosition, FVector::OneVector };
	}

	return FTransform::Identity;
}

void AOvrlWeaponInstance::SpawnImpactVFX(const FHitResult& HitData)
{
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType1; // 1 -> Concrete

	// Get impact surface
	if (HitData.PhysMaterial.IsValid())
	{
		SurfaceType = HitData.PhysMaterial->SurfaceType;
	}

	// Get different effects depending on the surface type
	FBulletImpactEffects ImpactEffects = BulletImpactEffects.FindRef(SurfaceType);

	// Spawn impact decal effect
	ensureMsgf(ImpactEffects.ImpactDecal, TEXT("Did you forget to set the Effect in the map?"));
	SpawnEffect(ImpactEffects.ImpactDecal, SurfaceType, HitData);

	// Spawn bullet impact effect
	ensureMsgf(ImpactEffects.ImpactEffect, TEXT("Did you forget to set the Effect in the map?"));
	SpawnEffect(ImpactEffects.ImpactEffect, SurfaceType, HitData);

	// Play impact sound
	ensureMsgf(ImpactEffects.ImpactSound, TEXT("Did you forget to set the Effect in the map?"));
	UGameplayStatics::PlaySoundAtLocation(this, ImpactEffects.ImpactSound, HitData.ImpactPoint);
}

void AOvrlWeaponInstance::SpawnEffect(UNiagaraSystem* Effect, EPhysicalSurface SurfaceType, const FHitResult& HitData)
{
	UNiagaraComponent* EffectNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, HitData.ImpactPoint, FRotator::ZeroRotator, FVector::OneVector);
	if (ensure(EffectNiagaraComp))
	{
		// Update Niagara FX params
		TArray<FVector> HitPositions;
		HitPositions.Add(HitData.ImpactPoint);

		TArray<FVector> HitNormals;
		HitNormals.Add(HitData.ImpactNormal);

		TArray<int32> SurfaceTypes;
		SurfaceTypes.Add(SurfaceType);

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(EffectNiagaraComp, "User.ImpactPositions", HitPositions);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(EffectNiagaraComp, "User.ImpactNormals", HitNormals);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(EffectNiagaraComp, "User.ImpactSurfaces", SurfaceTypes);
		EffectNiagaraComp->SetNiagaraVariableInt("User.NumberOfHits", 1);
	}
}
