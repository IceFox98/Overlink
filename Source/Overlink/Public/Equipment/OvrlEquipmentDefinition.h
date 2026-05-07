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
	// Sets of ability to grant to the entity that will equip this item (player/enemy).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition")
	TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySetsToGrantToOwner;

	// If true, the item will be immediately added to the quick slots and set as currently active.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition")
	bool bSetAsActiveSlotOnAdded = false;

	// If true, an actor instance of the item will be spawned and attached to the player.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition")
	bool bShouldSpawnEquipmentInstance = false;

	// Sets of ability to grant to the item itself when equipped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySetsToGrantToItem;

	// Name of the socket/bone to which the equipment instance will be attached to.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	FName AttachSocketName;

	// The Actor class to spawn when this item is equipped-
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	TSubclassOf<AOvrlEquipmentInstance> EquipmentClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	TSubclassOf<UOvrlLinkedAnimInstance> OverlayAnimInstance;

	// Should player play a montage when equip this item?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance", EditConditionHides))
	bool bPlayMontageOnEquip = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance && bPlayMontageOnEquip", EditConditionHides))
	TObjectPtr<UAnimMontage> EquipMontage;

	// Name of the notify used in the equipment montage, to hide the equipped item at specific time.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Definition", meta=(EditCondition = "bShouldSpawnEquipmentInstance && bPlayMontageOnEquip", EditConditionHides))
	FName EquipNotifyName = FName("ChangeItemNotify");
};
