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

// A single entry in an inventory
USTRUCT(BlueprintType)
struct FOvrlItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UOvrlItemInstance> Instance;

	UPROPERTY(BlueprintReadOnly)
	int32 Count = 0;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOvrlInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOvrlInventoryComponent();

public:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component", meta=(AdvancedDisplay = 2))
	UOvrlItemInstance* AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void AddItem(UOvrlItemInstance* Item, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void DropItem(UOvrlItemInstance* ItemToDrop);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void RemoveItem(UOvrlItemInstance* ItemToRemove, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	FORCEINLINE AOvrlEquipmentInstance* GetEquippedItem() const { return EquippedItem; }

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	FOvrlItemEntry FindFirstItemEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const;

	// Searches an item definition type for a matching stat and returns the value, or 0 if not found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	static int32 GetDefaultStatFromItemDef(const TSubclassOf<UOvrlItemDefinition> WeaponItemClass, FGameplayTag StatTag);

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
	// Items that will be automatically added on begin play
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TMap<TSubclassOf<UOvrlItemDefinition>, int32> InitialItems;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<FOvrlItemEntry> ItemEntries;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TArray<AOvrlEquipmentInstance*> EquippedItems;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	TObjectPtr<AOvrlEquipmentInstance> EquippedItem;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
	int32 SelectedIndex;

private:
	FTimerHandle TimerHandle_EquipItem;
};
