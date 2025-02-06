// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OBM_EquipmentDefinition.generated.h"

class AOBM_EquipmentInstance;
class UOBM_AbilitySet;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, Const, Abstract)
class OVERLINK_API UOBM_EquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	/**The mesh to display for this items pickup*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AOBM_EquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TArray<TObjectPtr<UOBM_AbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		FTransform RelativeTransform;
};
