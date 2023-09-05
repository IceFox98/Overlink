//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "Components/ActorComponent.h"
//#include "InventoryComponent.generated.h"
//
//class UItem;
//class APuzzleGuyEquipmentInstance;
//class UEquippableItem;
//class UPuzzleGuyPickupDefinition;
//class UPuzzleGuyAbilitySystemComponent;
//
//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//class ONEBULLETMAN_API UInventoryComponent : public UActorComponent
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this component's properties
//	UInventoryComponent();
//
//protected:
//
//	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	//	TArray<UItem*> Items;
//
//	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	//	TArray<APuzzleGuyEquipmentInstance*> EquippedItems;
//
//	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
//	//	APuzzleGuyEquipmentInstance* SelectedItem;
//
//	int32 SelectedIndex;
//
//protected:
//	// Called when the game starts
//	virtual void BeginPlay() override;
//
//public:
//	// Called every frame
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//	//UItem* AddItemFromPickup(UPuzzleGuyPickupDefinition* PickupDef);
//
//	UFUNCTION(BlueprintCallable)
//		void RemoveItem(int32 IndexToRemove);
//
//	UFUNCTION(BlueprintCallable)
//		void NextItem();
//
//	UFUNCTION(BlueprintCallable)
//		void PrevItem();
//
//	//UFUNCTION(BlueprintCallable, BlueprintPure)
//	//	FORCEINLINE APuzzleGuyEquipmentInstance* GetSelectedItem() const { return SelectedItem; }
//
//	UFUNCTION(BlueprintCallable)
//		AActor* DropSelectedItem();
//
//private:
//
//	void AddEquippableItem(UEquippableItem* NewItem, UPuzzleGuyPickupDefinition* PickupDef);
//
//	void EquipItemInSlot();
//	void UnequipCurrentItem();
//
//	void SetActiveSlotIndex(int32 NewIndex);
//
//	USceneComponent* GetItemHoldingPoint() const;
//	USceneComponent* GetItemAttachComp() const;
//
//	//UPuzzleGuyAbilitySystemComponent* GetAbilitySystemComponent() const;
//
//
//};
