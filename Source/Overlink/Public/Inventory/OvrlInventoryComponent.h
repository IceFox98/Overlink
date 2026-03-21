// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlItemPickupActor.h"
#include "Components/ActorComponent.h"
#include "OvrlInventoryComponent.generated.h"

class UOvrlAbilitySystemComponent;
class UOvrlItemDefinition;
class UOvrlItemInstance;
class AOvrlEquipmentInstance;
class AOvrlItemPickupActor;

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

USTRUCT(BlueprintType)
struct FOvrlInitialItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UOvrlItemDefinition> ItemDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AOvrlItemPickupActor> PickupClass = AOvrlItemPickupActor::StaticClass();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Count = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UOvrlItemInstance*, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, const FOvrlItemEntry&, UpdatedItem);
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
	virtual void BeginPlay() override;

public:
	UOvrlItemInstance* AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, TSubclassOf<AOvrlItemPickupActor> PickupClass, int32 Count = 1);

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	AOvrlEquipmentInstance* FindFirstEquipmentInstanceByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component", meta=(AdvancedDisplay="bCreateItemIfMissing"))
	void AddItemCount(UOvrlItemInstance* Item, int32 CountToAdd, bool bCreateItemIfMissing);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component",
		meta=(PickupClass="OvrlItemPickupActor", AutoCreateRefTerm="PickupClass", AdvancedDisplay="bCreateItemIfMissing, PickupClass"))
	void AddItemCountByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 CountToAdd,
	                              bool bCreateItemIfMissing, const TSubclassOf<AOvrlItemPickupActor>& PickupClass);

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

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemUpdated OnItemUpdated;

protected:
	// Items that will be automatically added on begin play
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FOvrlInitialItemData> InitialItems;

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
