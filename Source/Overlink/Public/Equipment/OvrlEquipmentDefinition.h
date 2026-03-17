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
	// Sets of ability to grant to the player when equips this item
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition")
	TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySetsToGrant;

	// Can this item be equipped in the player quick slot?
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition")
	bool bAllowQuickSlot = false;

	// If true, an actor instance of the item will be spawned and attached to the player
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition")
	bool bShouldSpawnEquipmentInstance = false;
	
	// The Actor class to spawn when this item is equipped
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	TSubclassOf<AOvrlEquipmentInstance> EquipmentClass;

	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	TSubclassOf<UOvrlLinkedAnimInstance> OverlayAnimInstance;

	// Should player play a montage when equip this item?
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	bool bPlayMontageOnEquip = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance && bPlayMontageOnEquip", EditConditionHides))
	TObjectPtr<UAnimMontage> EquipMontage;

	// Name of the notify used in the equipment montage, to hide the equipped item at specific time.
	UPROPERTY(EditAnywhere, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance && bPlayMontageOnEquip", EditConditionHides))
	FName EquipNotifyName = FName("ChangeItemNotify");
};
