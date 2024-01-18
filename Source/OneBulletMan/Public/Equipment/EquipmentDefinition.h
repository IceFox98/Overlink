// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class AEquipmentInstance;
class UOBM_AbilitySet;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, Const, Abstract)
class ONEBULLETMAN_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	/**The mesh to display for this items pickup*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TArray<TObjectPtr<UOBM_AbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		FTransform RelativeTransform;
};
