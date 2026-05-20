// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlSaveTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "OvrlSaveGameSubsystem.generated.h"

struct FOvrlSaveGameArchive : public FObjectAndNameAsStringProxyArchive
{
	FOvrlSaveGameArchive(FArchive& InInnerArchive)
		: FObjectAndNameAsStringProxyArchive(InInnerArchive, false)
	{
		ArIsSaveGame = true;
		// If true, even though the property values have not been altered from the default value, the serializer will still serialize these properties.
		ArNoDelta = true;
	}
};

class UOvrlSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGame, class UOvrlSaveGame*, SaveObject);

UCLASS(meta=(DisplayName="Ovrl SaveGame System"))
class OVERLINK_API UOvrlSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Initialize Subsystem, good moment to load in SaveGameSettings variables
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void SerializeObject(UObject* Object, TArray<uint8>& OutResult);
	void DeserializeObject(const TArray<uint8>& Data, UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void CreateNewSaveGame(FString SlotName);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void SaveCurrentSlot();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void LoadGame(FString SlotName);

	void LoadPlayerData(APawn* Player);
	void OnPostPlayerPossessed();

	FActorSaveData GetPlayerSaveData() const;

protected:
	void ExecuteOnPreSaveSafe(UObject* Target);
	void ExecuteOnSaveSafe(UObject* Target);
	void ExecuteOnPreLoadSafe(UObject* Target);
	void ExecuteOnLoadSafe(UObject* Target);

private:
	void PopulateCurrentSlot();
	void PopulateFromCurrentSlot();

	void LoadActor(AActor* Actor, const FActorSaveData& ActorSaveData);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSaveGame OnGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGame OnGameSaved;

protected:
	// Name of slot to save/load to disk. Filled by SaveGameSettings (can be overriden from GameMode's InitGame())
	FString CurrentSlotName;

	UPROPERTY(Transient)
	TObjectPtr<UOvrlSaveGame> CurrentSaveGame;
};
