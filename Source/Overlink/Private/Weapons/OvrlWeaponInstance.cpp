// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlWeaponInstance.h"
#include "Core/OvrlDamageable.h"

#include "Components/SphereComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"

AOvrlWeaponInstance::AOvrlWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);

	WeaponSwayRotationLimit = FVector2D(3.f, 10.f);
	WeaponSwayRotationSpeed = 5.f;
}

void AOvrlWeaponInstance::BeginPlay()
{
	Super::BeginPlay();

	WeaponMesh->OnComponentHit.AddDynamic(this, &AOvrlWeaponInstance::OnWeaponHit);
}

void AOvrlWeaponInstance::Fire(const FHitResult& HitData)
{
	K2_OnFire(HitData);

	if (HitData.GetActor() && HitData.GetActor()->Implements<UOvrlDamageable>())
	{
		OnHitSomething.ExecuteIfBound(HitData);
	}
}

void AOvrlWeaponInstance::Reload()
{
	// No implementation
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
	UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BulletImpactVFX, HitData.ImpactPoint, FRotator::ZeroRotator, FVector::OneVector);

	TArray<FVector> HitPositions;
	HitPositions.Add(HitData.ImpactPoint);

	TArray<FVector> HitNormals;
	HitNormals.Add(HitData.ImpactNormal);

	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;

	if (HitData.PhysMaterial.IsValid())
	{
		SurfaceType = HitData.PhysMaterial->SurfaceType;
	}

	TArray<int32> SurfaceTypes;
	SurfaceTypes.Add(2);

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayPosition(NiagaraComp, "User.ImpactPositions", HitPositions);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(NiagaraComp, "User.ImpactNormals", HitNormals);
	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(NiagaraComp, "User.ImpactSurfaces", SurfaceTypes);
	NiagaraComp->SetNiagaraVariableInt("User.NumberOfHits", 1);
}
