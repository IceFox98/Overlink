// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/OBM_ItemDefinition.h"
#include "OBM_ItemFragment_EquippableItem.generated.h"

class UOBM_EquipmentDefinition;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOBM_ItemFragment_EquippableItem : public UOBM_ItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UOBM_EquipmentDefinition> EquipmentDefinition;
};
