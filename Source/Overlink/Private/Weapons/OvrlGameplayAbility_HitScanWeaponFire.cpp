// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlGameplayAbility_HitScanWeaponFire.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

#if ENABLE_DRAW_DEBUG
#include "KismetTraceUtils.h"
#endif

#include "OvrlUtils.h"

void UOvrlGameplayAbility_HitScanWeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bCanActivateAbility = CommitAbility(Handle, ActorInfo, ActivationInfo);

	//if (bCanActivateAbility)
	if (AOvrlRangedWeaponInstance* WeaponInstance = Cast<AOvrlRangedWeaponInstance>(GetCurrentSourceObject()))
	{
		StartRangedWeaponTargeting();

		// The fire rate is managed by GAS: if the ability is still active (weapon firing), it can't be acivate again until you call EndAbility
		const float TimeBetweenShots = WeaponInstance->GetTimeBetweenShots();
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireCooldown, this, &UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown, TimeBetweenShots, false);
	}
}

void UOvrlGameplayAbility_HitScanWeaponFire::StartRangedWeaponTargeting()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	FGameplayAbilityTargetDataHandle HitTargetData;

	if (PC)
	{
		if (AOvrlRangedWeaponInstance* WeaponInstance = Cast<AOvrlRangedWeaponInstance>(GetCurrentSourceObject()))
		{
			const FVector BulletDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(PC->PlayerCameraManager->GetActorForwardVector(), WeaponInstance->GetSpreadAngle());

			// Trace from center of the camera to the weapon max range
			const FVector TraceStart = PC->PlayerCameraManager->GetCameraLocation();
			const FVector TraceEnd = TraceStart + BulletDirection * WeaponInstance->GetMaxDamageRange();

			FCollisionQueryParams Params;
			Params.AddIgnoredActor(WeaponInstance);
			Params.AddIgnoredActor(WeaponInstance->GetOwner());

			EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

			FHitResult HitResult;
			GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, Params);

#if ENABLE_DRAW_DEBUG
			const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(WeaponInstance->GetOwner(), "Ovrl.Weapons");

			if (bDebugEnabled)
				DrawDebugLineTraceSingle(GetWorld(), TraceStart, TraceEnd, EDrawDebugTrace::ForDuration, HitResult.bBlockingHit, HitResult, FLinearColor::Red, FLinearColor::Green, 5.f);
#endif

			if (!HitResult.bBlockingHit)
			{
				// Save the TraceEnd as ImpactPoint so that we can use it for other calculations
				HitResult.ImpactPoint = TraceEnd;
			}

			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = HitResult;
			HitTargetData.Add(NewTargetData);
		}
	}

	K2_OnRangedWeaponTargetDataReady(HitTargetData);
}

void UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}