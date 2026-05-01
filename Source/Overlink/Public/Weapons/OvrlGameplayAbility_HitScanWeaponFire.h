// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OvrlGameplayAbility.h"

#include "OvrlGameplayAbility_HitScanWeaponFire.generated.h"

class AOvrlRangedWeaponInstance;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlGameplayAbility_HitScanWeaponFire : public UOvrlGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ovrl Hit-Scan Weapon Fire")
	void StartRangedWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Hit-Scan Weapon Fire", meta = (DisplayName = "On Ranged Weapon Target Data Ready"))
	void K2_OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	virtual void OnAbilityInputStarted() override;
	virtual void OnAbilityInputReleased() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Hit-Scan Weapon Fire")
	AOvrlRangedWeaponInstance* GetWeaponInstance() const;

protected:
	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void HandleWeaponFire();
	void ResetFireCooldown();
	void StopWeaponFire();

public:
	UPROPERTY(EditAnywhere, Category = "Ovrl Hit-Scan Weapon Fire")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

private:
	FTimerHandle TimerHandle_FireCooldown;
	
	double StartFireTime;
	bool bHasPendingShot;
	float FireCooldownDuration;
};
