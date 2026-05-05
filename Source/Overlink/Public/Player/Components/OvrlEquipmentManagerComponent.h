// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OvrlEquipmentManagerComponent.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlItemInstance;
class UOvrlInventoryComponent;
class UOvrlAbilitySystemComponent;
class AOvrlEquipmentInstance;
class UOvrlItemDefinition;

USTRUCT(BlueprintType)
struct FQuickSlotEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<UOvrlItemInstance> ItemInstance = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AOvrlEquipmentInstance> EquipmentInstance = nullptr;
	
	void Invalidate()
	{
		ItemInstance = nullptr;
		EquipmentInstance = nullptr;
	};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipInstanceChanged, AOvrlEquipmentInstance*, EquippedInstance);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERLINK_API UOvrlEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeFromInventory(UOvrlInventoryComponent* InventoryComponent);

	UFUNCTION(BlueprintCallable, meta=(AdvancedDisplay="bForceSet"))
	void SetActiveSlotIndex(int32 NewIndex, bool bForceSet = false);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AOvrlEquipmentInstance* FindFirstEquipmentInstanceByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const;

	int32 AddItemToQuickSlots(UOvrlItemInstance* Item);

protected:
	UFUNCTION()
	void OnInventoryItemAdded(UOvrlItemInstance* AddedItem);

	UFUNCTION()
	void OnInventoryItemRemoved(UOvrlItemInstance* RemovedItem);

	UFUNCTION()
	void OnItemDropped();

	int32 GetFirstAvailableQuickSlotIndex() const;

	UOvrlAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

private:
	UFUNCTION()
	void SetActiveSlotIndex_Internal(int32 NewIndex);

	void EquipItem(UOvrlItemInstance* Item);
	void EquipItemInSlot();
	void UnequipItemInSlot();
	void UnequipItem(UOvrlItemInstance* ItemToUnequip);

	const UOvrlEquipmentDefinition* GetItemEquipmentDefinition(UOvrlItemInstance* Item) const;

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnEquipInstanceChanged OnEquipInstanceChanged;
	
protected:
	// Number of quick slots available.
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = 1, ClampMax = 30))
	int32 NumQuickSlots = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 QuickSlotIndex = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<UOvrlItemInstance*> ItemInstances;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FQuickSlotEntry> QuickSlotEntries;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FQuickSlotEntry CurrentActiveSlotEntry;

private:
	FTimerHandle TimerHandle_EquipItem;
};
