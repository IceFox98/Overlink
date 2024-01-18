// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"
#include "OBM_GameplayAbility_WeaponFire.generated.h"

/**
 *
 */
UCLASS()
class ONEBULLETMAN_API UOBM_GameplayAbility_WeaponFire : public UOBM_GameplayAbility
{
	GENERATED_BODY()

public:

	UOBM_GameplayAbility_WeaponFire();

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Fire")
		TSubclassOf<UGameplayEffect> GE_Damage;
};
