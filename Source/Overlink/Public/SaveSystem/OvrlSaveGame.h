// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSystem/OvrlSaveTypes.h"
#include "OvrlSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	void Initialize(const FString& InSlotName);
	void Reset();

public:
	UPROPERTY()
	FString SlotName;
	
	UPROPERTY()
	FString LevelName;

	UPROPERTY()
	TArray<uint8> GameState;

	UPROPERTY()
	TArray<uint8> PlayerController;

	UPROPERTY()
	TArray<uint8> PlayerState;

	UPROPERTY()
	FActorSaveData PlayerData;

	UPROPERTY()
	FRotator ControlRotation;

	// Only actor placed in the level
	UPROPERTY()
	TMap<FName, FActorSaveData> LevelActors;

	// Only actor spawned at runtime
	UPROPERTY()
	TArray<FActorSaveData> SpawnedActors;
};

UCLASS()
class OVERLINK_API UOvrlSaveGameSlots : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FSaveSlotMetadata> SaveSlotMetas;

	UPROPERTY()
	int32 LastPlayerSlotIndex;
};
