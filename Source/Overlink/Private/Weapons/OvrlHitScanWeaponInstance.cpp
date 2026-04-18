// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlHitScanWeaponInstance.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void AOvrlHitScanWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	// Apply damage to hit pawn
	if (const APawn* HitPawn = Cast<APawn>(HitData.GetActor()))
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitPawn);
		UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());

		if (InstigatorASC)
		{
			// Create GE context and add the hit result of the weapon
			FGameplayEffectContextHandle ContextHandle = InstigatorASC->MakeEffectContext();
			ContextHandle.AddHitResult(HitData);
			ContextHandle.AddInstigator(this, this);

			const FGameplayEffectSpecHandle SpecHandle = InstigatorASC->MakeOutgoingSpec(
				GE_Damage,
				1.f,
				ContextHandle
			);
			
			if (!SpecHandle.IsValid()) return;
			
			const float FinalDamage = ComputeDamage(HitData);
			SpecHandle.Data->SetSetByCallerMagnitude(DamageMagnitudeTag, -FinalDamage);
			InstigatorASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
		}
	}

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
