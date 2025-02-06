// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OBM_InventoryComponent.generated.h"

class UOBM_AbilitySystemComponent;
class UOBM_ItemDefinition;
class UOBM_ItemInstance;
class AOBM_EquipmentInstance;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOBM_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOBM_InventoryComponent();

	//protected:
	//	// Called when the game starts
	//	virtual void BeginPlay() override;
	//
	//public:
	//	// Called every frame
	//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		//UItem* AddItemFromPickup(UPuzzleGuyPickupDefinition* PickupDef);

		//UFUNCTION(BlueprintCallable)
		//	void RemoveItem(int32 IndexToRemove);

		//UFUNCTION(BlueprintCallable)
		//	void NextItem();

		//UFUNCTION(BlueprintCallable)
		//	void PrevItem();

		//UFUNCTION(BlueprintCallable, BlueprintPure)
		//	FORCEINLINE APuzzleGuyEquipmentInstance* GetSelectedItem() const { return SelectedItem; }

		//UFUNCTION(BlueprintCallable)
		//	AActor* DropSelectedItem();

	UFUNCTION(BlueprintCallable)
		UOBM_ItemInstance* AddItemDefinition(TSubclassOf<UOBM_ItemDefinition> ItemDef, int32 StackCount);

private:

	UOBM_AbilitySystemComponent* GetAbilitySystemComponent() const;

	//void EquipItemInSlot();
	//void UnequipCurrentItem();

	UFUNCTION(BlueprintCallable)
		void SetActiveSlotIndex(int32 NewIndex);

	//USceneComponent* GetItemHoldingPoint() const;
	//USceneComponent* GetItemAttachComp() const;

	//UPuzzleGuyAbilitySystemComponent* GetAbilitySystemComponent() const;

	void EquipItemInSlot();
	void UnequipItemInSlot();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UOBM_ItemDefinition*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<AOBM_EquipmentInstance*> EquippedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AOBM_EquipmentInstance* SelectedItem;

	int32 SelectedIndex;

};
