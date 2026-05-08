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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquipped, UOvrlItemInstance*, EquippedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUnequipped, UOvrlItemInstance*, UnequippedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActiveSlotChanged, const FQuickSlotEntry&, ActiveSlotEntry);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class OVERLINK_API UOvrlEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Manager Component")
	void InitializeFromInventory(UOvrlInventoryComponent* InventoryComponent);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Manager Component", meta=(AdvancedDisplay="bForceSet"))
	void SetActiveSlotIndex(int32 NewIndex, bool bForceSet = false);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Equipment Manager Component")
	const FQuickSlotEntry& FindFirstQuickSlotEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const;

	int32 AddItemToQuickSlots(UOvrlItemInstance* Item);
	void RemoveItemFromQuickSlots(UOvrlItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Equipment Manager Component")
	FORCEINLINE UOvrlItemInstance* GetActiveSlotItemInstance() const { return CurrentActiveSlotEntry.ItemInstance; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Equipment Manager Component")
	FORCEINLINE AOvrlEquipmentInstance* GetActiveSlotEquipInstance() const { return CurrentActiveSlotEntry.EquipmentInstance; };

protected:
	UFUNCTION()
	void OnInventoryItemAdded(UOvrlItemInstance* AddedItem);

	UFUNCTION()
	void OnInventoryItemRemoved(UOvrlItemInstance* RemovedItem);

	int32 GetFirstAvailableQuickSlotIndex() const;

	UOvrlAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

private:
	UFUNCTION()
	void SetActiveSlotIndex_Internal(int32 NewIndex);

	void EquipItem(UOvrlItemInstance* Item);
	void EquipItemInSlot();
	void UnequipItemInSlot();
	void UnequipItem(UOvrlItemInstance* ItemToUnequip);

public:
	UPROPERTY(BlueprintAssignable, Category = "Ovrl Equipment Manager Component")
	FOnItemEquipped OnItemEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Equipment Manager Component")
	FOnItemUnequipped OnItemUnequipped;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Equipment Manager Component")
	FOnActiveSlotChanged OnActiveSlotChanged;

protected:
	// Number of quick slots available.
	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = 1, ClampMax = 30))
	int32 NumQuickSlots = 1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 QuickSlotIndex = -1;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<UOvrlItemInstance*> ItemInstances;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FQuickSlotEntry> QuickSlotEntries;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	FQuickSlotEntry CurrentActiveSlotEntry;

private:
	FTimerHandle TimerHandle_EquipItem;
};
