// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/OvrlItemDefinition.h"
#include "OvrlItemFragment_PickupableItem.generated.h"

class UOvrlPickupDefinition;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlItemFragment_PickupableItem : public UOvrlItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UOvrlPickupDefinition> PickupDefinition;
};
