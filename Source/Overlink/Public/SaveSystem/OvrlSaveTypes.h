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
	FTransform RelativeTransform;

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
	UPROPERTY(SaveGame)
	FGuid ItemGuid;

	UPROPERTY(SaveGame)
	TSubclassOf<UOvrlItemDefinition> ItemDefinition;

	UPROPERTY(SaveGame)
	TArray<FGameplayTagStack> Stacks;

	UPROPERTY(SaveGame)
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

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	// Contains all 'SaveGame' marked variables of the Actor
	UPROPERTY()
	TArray<uint8> ByteData;

	UPROPERTY()
	TArray<FComponentSaveData> ComponentsSaveData;

public:
	bool IsValid() const
	{
		return !Name.IsNone();
	}
};

USTRUCT(BlueprintType)
struct FSaveSlotMetadata
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName;
	
	UPROPERTY()
	FDateTime Date;
};
