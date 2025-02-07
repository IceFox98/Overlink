// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/OvrlItemDefinition.h"
#include "GameplayTagContainer.h"
#include "OvrlItemFragment_SetStats.generated.h"

class UOvrlItemInstance;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlItemFragment_SetStats : public UOvrlItemFragment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UOvrlItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
	
};
