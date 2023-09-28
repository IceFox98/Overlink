// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemDefinition.h"
#include "GameplayTagContainer.h"
#include "ItemFragment_SetStats.generated.h"

class UItemInstance;

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API UItemFragment_SetStats : public UItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
	
};
