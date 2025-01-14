// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"

#include "OBM_GameplayAbility_HitScanWeaponFire.generated.h"

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API UOBM_GameplayAbility_HitScanWeaponFire : public UOBM_GameplayAbility
{
	GENERATED_BODY()

public:

	UOBM_GameplayAbility_HitScanWeaponFire();

public:

	UFUNCTION(BlueprintCallable, Category = "OBM|Hit-Scan Weapon Fire")
		void StartRangedWeaponTargeting();

	// Called when target data is ready
	UFUNCTION(BlueprintImplementableEvent, Category = "OBM|Hit-Scan Weapon Fire")
		void OnRangedWeaponTargetDataReady(const FHitResult& TargetData);

protected:

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Hit-Scan Weapon Fire")
		TSubclassOf<UGameplayEffect> GE_Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Hit-Scan Weapon Fire")
		float TraceMaxDistance;
};
