// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Interfaces/OvrlSaveableObject.h"
#include "GameFramework/Actor.h"
#include "SaveSystem/OvrlSaveTypes.h"
#include "OvrlItemPickupActor.generated.h"

class UOvrlItemDefinition;
class UCapsuleComponent;
class UStaticMeshComponent;
class UOvrlItemInstance;
class UOvrlInventoryComponent;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API AOvrlItemPickupActor : public AActor, public IOvrlSaveableObject
{
private:
	GENERATED_BODY()

public:
	AOvrlItemPickupActor();

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Pickup Actor")
	void HandlePickup(APawn* InInstigator);

	// Returns true if this Pickup Actor has an existing cached item.
	// The cached item is usually valid when the item was dropped from the player/enemy (see DropItem of Inventory Component)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Item Pickup Actor")
	FORCEINLINE bool HasCachedItem() const { return CachedItemInstance != nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Item Pickup Actor")
	FORCEINLINE UOvrlItemInstance* GetCachedItem() const { return CachedItemInstance; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Item Pickup Actor")
	UOvrlItemDefinition* GetItemDefinition() const;

	void SetCachedItemInstance(UOvrlItemInstance* ItemToCache, int32 InQuantity = 1);

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Item Pickup Actor", meta=(ReturnDisplayName="Handled"))
	bool ManageDuplicatedItem(TSubclassOf<UOvrlItemDefinition> DuplicatedItemClass, UOvrlItemInstance* ExistingItem, APawn* ReceivingPawn);

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Item Pickup Actor")
	void Drop();

#if WITH_EDITOR
	// If pressed, all the Pickup Info will be copied to the relative pickup definition Asset of the selected item.
	// (See 'Ovrl Pickup Info' section of the Pickup definition asset).
	UFUNCTION(CallInEditor, Category = "Ovrl Item Pickup Actor", DisplayName = "Update Pickup Definition")
	void UpdatePickupDefinition() const;
#endif

protected:
	UFUNCTION()
	void OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AddItemToInventory(UOvrlInventoryComponent* TargetInventoryComponent) const;
	void RefreshData() const;

	// ---- IOvrlSaveableObject interface
	virtual void OnPreSave_Implementation() override;
	virtual void OnLoad_Implementation() override;
	// ---- IOvrlSaveableObject interface

public:
	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> PickupCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ItemMesh;

protected:
	// ------ ITEM ------

	// If true, the item will be added to the inventory even if there's already an instance of it.
	// Otherwise, you can manage the item pickup behavior using ManageDuplicatedItem() of this class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Pickup Actor")
	bool bAddDuplicatedItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Pickup Actor")
	TSubclassOf<UOvrlItemDefinition> ItemDefinitionClass;

	// The quantity assigned to the item when it will be added to the inventory.
	// E.g.: ID_HealthPotion - 20 -> In the inventory you will have 1 item entry (Health Potion) with 20 count
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Pickup Actor", meta = (ClampMin = 1))
	int32 Quantity = 1;

	// Reference used when an entity drops an item.
	UPROPERTY(VisibleInstanceOnly, Category = "Ovrl Item Pickup Actor")
	TObjectPtr<UOvrlItemInstance> CachedItemInstance;

private:
	UPROPERTY(SaveGame)
	FInventoryItemEntrySaveData SavedItem;
};
