// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OvrlItemPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UOvrlPickupDefinition;
class UOvrlItemInstance;
class UOvrlInventoryComponent;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API AOvrlItemPickupActor : public AActor
{
	GENERATED_BODY()

public:
	AOvrlItemPickupActor();

public:
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	void ShowItemMesh();

public:
	// Returns true if this Pickup Actor has an existing cached item.
	// The cached item is usually valid when the item was dropped from the player/enemy (see DropItem of Inventory Component)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Item Pickup Actor")
	FORCEINLINE bool HasCachedItem() const { return CachedItemInstance != nullptr; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Item Pickup Actor")
	FORCEINLINE UOvrlItemInstance* GetCachedItem() const { return CachedItemInstance; }

	FORCEINLINE void SetCachedItemInstance(UOvrlItemInstance* ItemToCache) { CachedItemInstance = ItemToCache; }

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Item Pickup Actor", meta=(ReturnDisplayName="Handled"))
	bool ManageDuplicatedItem(TSubclassOf<UOvrlItemDefinition> DuplicatedItemClass, UOvrlItemInstance* ExistingItem, APawn* ReceivingPawn);

protected:
	UFUNCTION()
	void OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void AddItemToInventory(UOvrlInventoryComponent* TargetInventoryComponent);

public:
	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> PickupCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	// ------ ITEM ------

	// If true, the item will be added to the inventory even if there's already an instance of it.
	// Otherwise, you can manage the item pickup behavior using ManageDuplicatedItem() of this class.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Pickup Actor")
	bool bAddDuplicatedItem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Pickup Actor")
	TObjectPtr<UOvrlPickupDefinition> ItemPickupDefinition;

protected:
	// Reference used when an entity drops an item.
	UPROPERTY()
	TObjectPtr<UOvrlItemInstance> CachedItemInstance;

};
