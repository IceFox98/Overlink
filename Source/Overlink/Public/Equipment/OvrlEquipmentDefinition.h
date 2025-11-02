// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OvrlEquipmentDefinition.generated.h"

class AOvrlEquipmentInstance;
class UOvrlAbilitySet;
class UOvrlLinkedAnimInstance;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API UOvrlEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:

	/**The mesh to display for this items pickup*/
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AOvrlEquipmentInstance> InstanceType;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySetsToGrant;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<UOvrlLinkedAnimInstance> OverlayAnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TObjectPtr<UAnimMontage> EquipMontage;
};
