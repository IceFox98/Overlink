// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "Inventory/OvrlItemInstance.h"
#include "OvrlSaveTypes.generated.h"

class UOvrlItemDefinition;

USTRUCT()
struct FComponentSaveData
{
	GENERATED_BODY()

public:
	// Identifier for which Actor this belongs to
	UPROPERTY()
	FName Name;

	// Used if it's at least a scene component
	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TSubclassOf<UActorComponent> ComponentClass;

	// Contains all 'SaveGame' marked variables of the Component
	UPROPERTY()
	TArray<uint8> ByteData;
};

USTRUCT()
struct FInventoryItemEntrySaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UOvrlItemDefinition> ItemDefinition;

	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	UPROPERTY()
	int32 Quantity;
};

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:
	// Identifier for which Actor this belongs to
	UPROPERTY()
	FName Name;

	// For movable Actors, keep location,rotation,scale.
	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	UObject* Outer;

	UPROPERTY()
	int32 OuterId;

	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	// Contains all 'SaveGame' marked variables of the Actor
	UPROPERTY()
	TArray<uint8> ByteData;

	UPROPERTY()
	TArray<FComponentSaveData> ComponentsSaveData;
};

USTRUCT()
struct FEntitySaveData : public FActorSaveData
{
	GENERATED_BODY()

public:
	FEntitySaveData() = default;

	FEntitySaveData(const FActorSaveData& InActorSaveData)
	{
		*this = InActorSaveData;
	}

	FEntitySaveData& operator=(const FActorSaveData& InActorSaveData)
	{
		Name = InActorSaveData.Name;
		Transform = InActorSaveData.Transform;
		Outer = InActorSaveData.Outer;
		OuterId = InActorSaveData.OuterId;
		ActorClass = InActorSaveData.ActorClass;
		ByteData = InActorSaveData.ByteData;
		ComponentsSaveData = InActorSaveData.ComponentsSaveData;
		return *this;
	}

public:
	UPROPERTY()
	TArray<FInventoryItemEntrySaveData> InventoryEntries;

};
