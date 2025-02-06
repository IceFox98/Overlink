// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/OBM_ItemDefinition.h"
#include "GameplayTagContainer.h"
#include "OBM_ItemFragment_SetStats.generated.h"

class UOBM_ItemInstance;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOBM_ItemFragment_SetStats : public UOBM_ItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UOBM_ItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
	
};
