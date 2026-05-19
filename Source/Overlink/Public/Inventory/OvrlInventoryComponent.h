// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlItemPickupActor.h"
#include "Components/ActorComponent.h"
#include "Core/Interfaces/OvrlSaveableObject.h"
#include "OvrlInventoryComponent.generated.h"

struct FInventoryItemEntrySaveData;
class UOvrlAbilitySystemComponent;
class UOvrlItemDefinition;
class UOvrlItemInstance;
class AOvrlEquipmentInstance;

// A single entry in an inventory
USTRUCT(BlueprintType)
struct FOvrlItemEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame)
	TObjectPtr<UOvrlItemInstance> Instance;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, SaveGame)
	int32 Quantity = 0;
};

USTRUCT(BlueprintType)
struct FOvrlInitialItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UOvrlItemDefinition> ItemDefinition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Quantity = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAdded, UOvrlItemInstance*, AddedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemRemoved, UOvrlItemInstance*, RemovedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemDropped, UOvrlItemInstance*, DroppedItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemUpdated, const FOvrlItemEntry&, UpdatedItem);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOvrlInventoryComponent : public UActorComponent, public IOvrlSaveableObject
{
private:
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

public:
	UOvrlItemInstance* AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 Quantity = 1);
	UOvrlItemInstance* CreateUniqueItem(TSubclassOf<UOvrlItemDefinition> ItemDefinition, FGuid ItemGuid = FGuid::NewGuid()) const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void AddItem(UOvrlItemInstance* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void DropItem(UOvrlItemInstance* ItemToDrop, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component")
	void RemoveItem(UOvrlItemInstance* ItemToRemove, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	FOvrlItemEntry FindFirstItemEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	FOvrlItemEntry FindFirstItemEntryByInstance(UOvrlItemInstance* Item) const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component", meta=(AdvancedDisplay="bCreateItemIfMissing"))
	void AddItemQuantity(UOvrlItemInstance* Item, int32 QuantityToAdd, bool bCreateItemIfMissing);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Inventory Component",
		meta=(AdvancedDisplay="bCreateItemIfMissing", ReturnDisplayName="Item Instance"))
	UOvrlItemInstance* AddItemQuantityByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 QuantityToAdd, bool bCreateItemIfMissing);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	FORCEINLINE TArray<FOvrlItemEntry> GetItemEntries() const { return ItemEntries; }

	// ---- IOvrlSaveableObject interface
	virtual void OnPreSave_Implementation() override;
	virtual void OnLoad_Implementation() override;
	// ---- IOvrlSaveableObject interface

public:
	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemUpdated OnItemUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
	FOnItemDropped OnItemDropped;

protected:
	// Items that will be automatically added on begin play
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FOvrlInitialItemData> InitialItems;

	UPROPERTY(VisibleInstanceOnly)
	TArray<FOvrlItemEntry> ItemEntries;

private:
	FTimerHandle TimerHandle_EquipItem;

	UPROPERTY(SaveGame)
	TArray<FInventoryItemEntrySaveData> SavedItems;
};
