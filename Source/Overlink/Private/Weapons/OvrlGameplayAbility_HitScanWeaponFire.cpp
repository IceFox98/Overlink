// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OBM_GameplayAbility_HitScanWeaponFire.h"
#include "Weapons/OBM_ProjectileWeapon.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UOBM_GameplayAbility_HitScanWeaponFire::UOBM_GameplayAbility_HitScanWeaponFire()
{
	TraceMaxDistance = 650.f;
}

void UOBM_GameplayAbility_HitScanWeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bCanActivateAbility = CommitAbility(Handle, ActorInfo, ActivationInfo);

	//if (bCanActivateAbility)
	{
		StartRangedWeaponTargeting();
	}
}

void UOBM_GameplayAbility_HitScanWeaponFire::StartRangedWeaponTargeting()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	FHitResult HitResult;

	if (PC)
	{
		if (AOBM_WeaponInstance* WeaponInstance = Cast<AOBM_WeaponInstance>(GetCurrentSourceObject()))
		{
			const FVector HitTraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector HitTraceEnd = HitTraceStart + PC->PlayerCameraManager->GetActorForwardVector() * TraceMaxDistance;

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(WeaponInstance);
			Params.AddIgnoredActor(WeaponInstance->GetOwner());

			GetWorld()->LineTraceSingleByChannel(HitResult, HitTraceStart, HitTraceEnd, ECC_Visibility, Params);
		}
	}

	OnRangedWeaponTargetDataReady(HitResult);
}
