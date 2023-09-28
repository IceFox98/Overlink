// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/ItemDefinition.h"
#include "ItemFragment_EquippableItem.generated.h"

class UEquipmentDefinition;

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API UItemFragment_EquippableItem : public UItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};
