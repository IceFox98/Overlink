// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlWeaponInstance.h"
#include "Core/OvrlDamageable.h"

#include "Components/SphereComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

AOvrlWeaponInstance::AOvrlWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(WeaponMesh);
}

void AOvrlWeaponInstance::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->OnComponentHit.AddDynamic(this, &AOvrlWeaponInstance::OnWeaponHit);
}

void AOvrlWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	WeaponMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
}

void AOvrlWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

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

void AOvrlWeaponInstance::PerformReload()
{
	bIsReloading = false;
}

void AOvrlWeaponInstance::ToggleWeaponPhysics(bool bEnable)
{
	if (bEnable)
	{
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetNotifyRigidBodyCollision(true);
	}
	else
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetAllPhysicsLinearVelocity(FVector::Zero());
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetNotifyRigidBodyCollision(false);
	}
}

// Move this to weapon-specific class?
void AOvrlWeaponInstance::OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ToggleWeaponPhysics(false);

	SetActorLocation(Hit.ImpactPoint);

	FVector NormalizedImpactNormal = Hit.ImpactNormal.GetSafeNormal();

	// Define an arbitrary vector (ensure it is not parallel to the normal)
	FVector ArbitraryVector = FVector::UpVector;
	if (FMath::Abs(NormalizedImpactNormal | ArbitraryVector) > 0.99f) // Check if nearly parallel
	{
		ArbitraryVector = FVector::RightVector;
	}

	// X-axis perpendicular to the impact normal
	FVector XAxis = FVector::CrossProduct(NormalizedImpactNormal, ArbitraryVector).GetSafeNormal();

	// Z-axis perpendicular to both Y-axis and X-axis
	FVector ZAxis = FVector::CrossProduct(XAxis, NormalizedImpactNormal).GetSafeNormal();

	FMatrix RotationMatrix(
		XAxis,                  // X-axis: perpendicular to the normal
		NormalizedImpactNormal, // Y-axis: aligned with the impact normal
		ZAxis,                  // Z-axis: computed for orthogonality
		FVector::ZeroVector     // Origin (irrelevant for rotation)
	);

	SetActorRotation(FQuat(RotationMatrix));
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
	ensureAlwaysMsgf(ImpactEffects.ImpactDecal, TEXT("Did you forget to set the Effect in the map?"));
	SpawnEffect(ImpactEffects.ImpactDecal, SurfaceType, HitData);

	// Spawn bullet impact effect
	ensureAlwaysMsgf(ImpactEffects.ImpactEffect, TEXT("Did you forget to set the Effect in the map?"));
	SpawnEffect(ImpactEffects.ImpactEffect, SurfaceType, HitData);

	// Play impact sound
	ensureAlwaysMsgf(ImpactEffects.ImpactSound, TEXT("Did you forget to set the Effect in the map?"));
	UGameplayStatics::PlaySoundAtLocation(this, ImpactEffects.ImpactSound, HitData.ImpactPoint);
}

void AOvrlWeaponInstance::SpawnEffect(UNiagaraSystem* Effect, EPhysicalSurface SurfaceType, const FHitResult& HitData)
{
	UNiagaraComponent* EffectNiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, Effect, HitData.ImpactPoint, FRotator::ZeroRotator, FVector::OneVector);
	if (ensureAlways(EffectNiagaraComp))
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
