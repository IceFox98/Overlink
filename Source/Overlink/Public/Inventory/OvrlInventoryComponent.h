// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OvrlInventoryComponent.generated.h"

class UOvrlAbilitySystemComponent;
class UOvrlItemDefinition;
class UOvrlItemInstance;
class AOvrlEquipmentInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, AOvrlEquipmentInstance*, EquippedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnequipped, AOvrlEquipmentInstance*, UnequippedItem);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOvrlInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOvrlInventoryComponent();

public:

	UFUNCTION(BlueprintCallable)
	void DropItem(UOvrlItemInstance* ItemToDrop);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(UOvrlItemInstance* ItemToRemove);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE AOvrlEquipmentInstance* GetEquippedItem() const { return EquippedItem; }

	UFUNCTION(BlueprintCallable)
	UOvrlItemInstance* AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable)
	void AddItem(UOvrlItemInstance* Item, int32 StackCount);
	
	UFUNCTION(BlueprintCallable)
	void SetActiveSlotIndex(int32 NewIndex);

private:

	UOvrlAbilitySystemComponent* GetAbilitySystemComponent() const;

	UFUNCTION()
	void SetActiveSlotIndex_Internal(int32 NewIndex);

	void EquipItemInSlot();
	void UnequipItemInSlot();
	void UnequipItem(AOvrlEquipmentInstance* ItemToUnequip) const;

public:

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemEquipped OnItemEquipped;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UOvrlItemInstance>> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AOvrlEquipmentInstance*> EquippedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<AOvrlEquipmentInstance> EquippedItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SelectedIndex;
	
private:
	
	FTimerHandle TimerHandle_EquipItem;
};
