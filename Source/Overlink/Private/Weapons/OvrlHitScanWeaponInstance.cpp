// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlHitScanWeaponInstance.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void AOvrlHitScanWeaponInstance::ProcessHit(const FHitResult& HitData)
{
	Super::ProcessHit(HitData);

	DealDamageToTargetFromHit(HitData);

	SpawnImpactVFX(HitData);
	SpawnTrailVFX(HitData);
}

void AOvrlHitScanWeaponInstance::SpawnTrailVFX(const FHitResult& HitData)
{
	if (ensure(BulletTrailVFX))
	{
		const FTransform MuzzleTransform = GetMuzzleTransform();
		const FRotator FXRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleTransform.GetLocation(), HitData.ImpactPoint);
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BulletTrailVFX, MuzzleTransform.GetLocation(), FXRotation, FVector::OneVector);
		ensure(NiagaraComp);

		TArray<FVector> HitPositions;
		HitPositions.Add(HitData.ImpactPoint);

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(NiagaraComp, "User.ImpactPositions", HitPositions);
	}
}
