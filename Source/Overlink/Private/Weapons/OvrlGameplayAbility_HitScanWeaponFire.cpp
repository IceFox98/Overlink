// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlGameplayAbility_HitScanWeaponFire.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

void UOvrlGameplayAbility_HitScanWeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bCanActivateAbility = CommitAbility(Handle, ActorInfo, ActivationInfo);

	//if (bCanActivateAbility)
	if (AOvrlRangedWeaponInstance* WeaponInstance = Cast<AOvrlRangedWeaponInstance>(GetCurrentSourceObject()))
	{
		StartRangedWeaponTargeting();
		
		//// Adding recoil and spread
		//WeaponInstance->AddSpread();

		// The fire rate is managed by GAS: if the ability is still active (weapon firing), it can't be acivate again until you call EndAbility
		const float TimeBetweenShots = WeaponInstance->GetTimeBetweenShots();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireCooldown, this, &UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown, TimeBetweenShots, false);
	}
}

void UOvrlGameplayAbility_HitScanWeaponFire::StartRangedWeaponTargeting()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	FHitResult HitResult;

	if (PC)
	{
		if (AOvrlRangedWeaponInstance* WeaponInstance = Cast<AOvrlRangedWeaponInstance>(GetCurrentSourceObject()))
		{
			// Trace from center of the camera to the weapon max range
			const FVector HitTraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector HitTraceEnd = HitTraceStart + PC->PlayerCameraManager->GetActorForwardVector() * WeaponInstance->GetMaxDamageRange();

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(WeaponInstance);
			Params.AddIgnoredActor(WeaponInstance->GetOwner());

			GetWorld()->LineTraceSingleByChannel(HitResult, HitTraceStart, HitTraceEnd, ECC_Visibility, Params);
		}
	}

	K2_OnRangedWeaponTargetDataReady(HitResult);
}

void UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}