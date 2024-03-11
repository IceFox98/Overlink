// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../AbilitySystem/OBM_AbilitySet.h"
#include "EquipmentInstance.generated.h"

class UEquipmentDefinition;
class UInventoryComponent;
class UItemInstance;

UCLASS()
class ONEBULLETMAN_API AEquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEquipmentInstance();

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

	UItemInstance* GetAssociatedItem() const { return AssociatedItem; };

protected:

	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Equipment Instance")
		TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
		UItemInstance* AssociatedItem;

private:

	friend class UInventoryComponent;

	// List of granted handles
	FOBM_AbilitySet_GrantedHandles GrantedHandles;
};
