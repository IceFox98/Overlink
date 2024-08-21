// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"
#include "OBM_GameplayAbility_WeaponFire.generated.h"

class AProjectile;

/**
 *
 */
UCLASS()
class ONEBULLETMAN_API UOBM_GameplayAbility_WeaponFire : public UOBM_GameplayAbility
{
	GENERATED_BODY()

public:

	UOBM_GameplayAbility_WeaponFire();

protected:

	/** Actually activate ability, do not call this directly */
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Fire")
		TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Fire")
		TSubclassOf<UGameplayEffect> GE_Damage;
};
