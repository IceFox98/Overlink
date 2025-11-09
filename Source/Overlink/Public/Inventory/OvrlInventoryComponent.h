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

	//protected:
	//	// Called when the game starts
	//	virtual void BeginPlay() override;
	//
public:
	//	// Called every frame
	//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		//UItem* AddItemFromPickup(UPuzzleGuyPickupDefinition* PickupDef);

	UFUNCTION(BlueprintCallable)
	void DropItem(UOvrlItemInstance* ItemToDrop);

	UFUNCTION(BlueprintCallable)
	void RemoveItem(UOvrlItemInstance* ItemToRemove);

	//UFUNCTION(BlueprintCallable)
	//	void RemoveCurrentItem();

	//UFUNCTION(BlueprintCallable)
	//	void NextItem();

	//UFUNCTION(BlueprintCallable)
	//	void PrevItem();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE AOvrlEquipmentInstance* GetEquippedItem() const { return EquippedItem; }

	//UFUNCTION(BlueprintCallable)
	//	AActor* DropSelectedItem();

	UFUNCTION(BlueprintCallable)
	UOvrlItemInstance* AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable)
	void AddItem(UOvrlItemInstance* Item, int32 StackCount);

private:

	UOvrlAbilitySystemComponent* GetAbilitySystemComponent() const;

	//void EquipItemInSlot();
	//void UnequipCurrentItem();

	UFUNCTION(BlueprintCallable)
	void SetActiveSlotIndex(int32 NewIndex);

	//USceneComponent* GetItemHoldingPoint() const;
	//USceneComponent* GetItemAttachComp() const;

	//UPuzzleGuyAbilitySystemComponent* GetAbilitySystemComponent() const;

	void EquipItemInSlot();
	//void EquipItem(AOvrlEquipmentInstance* ItemToEquip);
	void UnequipItemInSlot();
	void UnequipItem(AOvrlEquipmentInstance* ItemToUnequip);

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

	int32 SelectedIndex;

};
