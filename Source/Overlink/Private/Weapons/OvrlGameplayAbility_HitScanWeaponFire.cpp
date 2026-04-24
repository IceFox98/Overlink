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

	if (AOvrlRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		StartRangedWeaponTargeting();

		// The fire rate is managed by GAS: if the ability is still active (weapon firing), it can't be activated again until you call EndAbility
		const float TimeBetweenShots = WeaponInstance->GetTimeBetweenShots();
		if (TimeBetweenShots > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_FireCooldown, this, &UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown, TimeBetweenShots, false);
		}
		else
		{
			ResetFireCooldown();
		}
	}
}

void UOvrlGameplayAbility_HitScanWeaponFire::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	StopWeaponFire();
}

void UOvrlGameplayAbility_HitScanWeaponFire::StartRangedWeaponTargeting()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetCurrentSourceObject(), 0);

	// Useful struct that can handle multiple target hits
	FGameplayAbilityTargetDataHandle HitTargetData;

	if (PC)
	{
		if (AOvrlRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
		{
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(WeaponInstance);
			Params.AddIgnoredActor(WeaponInstance->GetOwner());
			Params.bReturnPhysicalMaterial = true;
			
			const int32 BulletsPerCartridge = WeaponInstance->GetBulletsPerCartridge();
			
			// Trace a trace for each bullets in the cartridge
			for (int32 BulletIndex = 0; BulletIndex < BulletsPerCartridge; BulletIndex++)
			{
				const FVector BulletDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(PC->PlayerCameraManager->GetActorForwardVector(), WeaponInstance->GetSpreadAngle());

				// Trace from center of the camera to the weapon max range
				const FVector TraceStart = PC->PlayerCameraManager->GetCameraLocation();
				const FVector TraceEnd = TraceStart + BulletDirection * WeaponInstance->GetMaxDamageRange();
			
				FHitResult HitResult;
				GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceCollisionChannel, Params);

#if ENABLE_DRAW_DEBUG
				const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(WeaponInstance->GetOwner(), "Ovrl.Weapons");

				if (bDebugEnabled)
					DrawDebugLineTraceSingle(GetWorld(), TraceStart, TraceEnd, EDrawDebugTrace::ForDuration, HitResult.bBlockingHit, HitResult, FLinearColor::Red, FLinearColor::Green, 5.f);
#endif

				if (!HitResult.bBlockingHit)
				{
					// Save the TraceEnd as ImpactPoint so that we can use it for other calculations
					HitResult.ImpactPoint = TraceEnd;
					HitResult.Location = TraceEnd;
				}

				FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
				NewTargetData->HitResult = HitResult;
				HitTargetData.Add(NewTargetData);
			}
		}
	}

	K2_OnRangedWeaponTargetDataReady(HitTargetData);
}

void UOvrlGameplayAbility_HitScanWeaponFire::ResetFireCooldown()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UOvrlGameplayAbility_HitScanWeaponFire::OnAbilityInputReleased()
{
	Super::OnAbilityInputReleased();

	StopWeaponFire();
}

void UOvrlGameplayAbility_HitScanWeaponFire::StopWeaponFire()
{
	if (AOvrlRangedWeaponInstance* WeaponInstance = GetWeaponInstance())
	{
		WeaponInstance->StopFire();
	}
}

AOvrlRangedWeaponInstance* UOvrlGameplayAbility_HitScanWeaponFire::GetWeaponInstance() const
{
	return Cast<AOvrlRangedWeaponInstance>(GetCurrentSourceObject());
}
