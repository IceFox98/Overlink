// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "OvrlEquipmentInstance.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlInventoryComponent;
class UOvrlItemInstance;
class UCurveVector;

UCLASS()
class OVERLINK_API AOvrlEquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOvrlEquipmentInstance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

public:

	virtual void OnEquipped();
	virtual void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Equipped"))
		void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Unequipped"))
		void K2_OnUnequipped();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
		FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

protected:

	void ApplyOverlayAnimation();

protected:

	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Instance")
		TSubclassOf<UOvrlEquipmentDefinition> EquipmentDefinitionClass;

	UPROPERTY()
		TObjectPtr<UOvrlItemInstance> AssociatedItem;

private:

	friend class UOvrlInventoryComponent;

	// List of granted handles
	FOvrlAbilitySet_GrantedHandles GrantedHandles;

	bool bIsEquipped = false;

	UPROPERTY()
		TObjectPtr<USceneComponent> TargetToFollow;

	FVector RelativeLocation;
};
