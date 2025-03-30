// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlHitScanWeaponInstance.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

AOvrlHitScanWeaponInstance::AOvrlHitScanWeaponInstance()
{

}

void AOvrlHitScanWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	// Apply damage to hit pawn
	if (APawn* HitPawn = Cast<APawn>(HitData.GetActor()))
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitPawn);
		UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());

		if (InstigatorASC)
		{
			UGameplayEffect* GameplayEffect = GE_Damage->GetDefaultObject<UGameplayEffect>();
			InstigatorASC->ApplyGameplayEffectToTarget(GameplayEffect, TargetASC, 1.f, InstigatorASC->MakeEffectContext());
		}
	}
}

void AOvrlHitScanWeaponInstance::SpawnFireVFX(const FHitResult& HitData)
{
	Super::SpawnFireVFX(HitData);

	if (BulletTrailVFX)
	{
		const FTransform MuzzleTransform = GetMuzzleTransform();
		const FRotator FXRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleTransform.GetLocation(), HitData.ImpactPoint);
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, BulletTrailVFX, MuzzleTransform.GetLocation(), FXRotation, FVector::OneVector);

		check(NiagaraComp);
		NiagaraComp->SetNiagaraVariablePosition("EndLocation", HitData.ImpactPoint);
	}
}
