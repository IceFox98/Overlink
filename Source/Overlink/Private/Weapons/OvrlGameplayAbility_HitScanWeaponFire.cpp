// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlGameplayAbility_HitScanWeaponFire.h"
#include "Weapons/OvrlProjectileWeapon.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UOvrlGameplayAbility_HitScanWeaponFire::UOvrlGameplayAbility_HitScanWeaponFire()
{
	TraceMaxDistance = 650.f;
}

void UOvrlGameplayAbility_HitScanWeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bCanActivateAbility = CommitAbility(Handle, ActorInfo, ActivationInfo);

	//if (bCanActivateAbility)
	{
		StartRangedWeaponTargeting();
	}
}

void UOvrlGameplayAbility_HitScanWeaponFire::StartRangedWeaponTargeting()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	FHitResult HitResult;

	if (PC)
	{
		if (AOvrlWeaponInstance* WeaponInstance = Cast<AOvrlWeaponInstance>(GetCurrentSourceObject()))
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
