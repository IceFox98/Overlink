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
	FEntitySaveData* GetPlayerData(APlayerState* InPlayerState);

public:
	UPROPERTY()
	FString SlotName;
	
	UPROPERTY()
	TArray<uint8> GameState;

	UPROPERTY()
	TArray<uint8> PlayerController;
	
	UPROPERTY()
	TArray<uint8> PlayerState;
	
	UPROPERTY()
	TArray<uint8> PlayerCharacter;
	
	// Can be either spawned or place in the level
	UPROPERTY()
	TArray<FEntitySaveData> EntitiesSaveData;

	// Only actor placed in the level
	UPROPERTY()
	TMap<FName, FActorSaveData> LevelActors;
	
	// Only actor spawned at runtime
	UPROPERTY()
	TArray<FActorSaveData> SpawnedActors;

	UPROPERTY()
	FRotator ControlRotation;

};
