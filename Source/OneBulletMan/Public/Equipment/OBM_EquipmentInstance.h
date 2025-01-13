// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/OBM_AbilitySet.h"
#include "OBM_EquipmentInstance.generated.h"

class UOBM_EquipmentDefinition;
class UOBM_InventoryComponent;
class UOBM_ItemInstance;

UCLASS()
class ONEBULLETMAN_API AOBM_EquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOBM_EquipmentInstance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual void OnEquipped();
	virtual void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment Instance", meta = (DisplayName = "OnEquipped"))
		void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Equipment Instance", meta = (DisplayName = "OnUnequipped"))
		void K2_OnUnequipped();

	FORCEINLINE UOBM_ItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

protected:

	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Equipment Instance")
		TSubclassOf<UOBM_EquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
		UOBM_ItemInstance* AssociatedItem;

private:

	friend class UOBM_InventoryComponent;

	// List of granted handles
	FOBM_AbilitySet_GrantedHandles GrantedHandles;

	bool bIsEquipped = false;

	UPROPERTY()
		TObjectPtr<USceneComponent> TargetToFollow;


	FVector RelativeLocation;
};
