// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlHitScanWeaponInstance.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

AOvrlHitScanWeaponInstance::AOvrlHitScanWeaponInstance()
{

}

void AOvrlHitScanWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	DrawDebugSphere(GetWorld(), HitData.ImpactPoint, 5.f, 5, FColor::Green, false, 2.f);

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
