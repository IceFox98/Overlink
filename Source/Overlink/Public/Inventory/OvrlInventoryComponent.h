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

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE AOvrlEquipmentInstance* GetEquippedItem() const { return EquippedItem; }

	//UFUNCTION(BlueprintCallable)
	//	AActor* DropSelectedItem();

	UFUNCTION(BlueprintCallable)
		UOvrlItemInstance* AddItemDefinition(TSubclassOf<UOvrlItemDefinition> ItemDef, int32 StackCount);

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
	void UnequipItemInSlot();

public:

	UPROPERTY(BlueprintAssignable, Category = "Ovrl Inventory Component")
		FOnItemEquipped OnItemEquipped;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UOvrlItemDefinition*> Items;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<AOvrlEquipmentInstance*> EquippedItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AOvrlEquipmentInstance* EquippedItem;

	int32 SelectedIndex;

};
