// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlSaveTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "OvrlSaveGameSubsystem.generated.h"

class UOvrlSaveGameSlots;
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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem", meta = (AdvancedDisplay = "SlotDisplayName"))
	void CreateNewSaveSlot(FString SlotDisplayName, bool bUseDefaultLevel, FSaveSlotMetadata& NewSlotMetadata, bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void SaveGame(FString SlotName);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void SaveCurrentSlot();

	// Load game data of the passed slot.
	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	void LoadGame(FString SlotName);
	void LoadSelectedSlot();

	void LoadPlayerData(APawn* Player);
	void OnPostPlayerPossessed();

	FString GetLastSaveSlotName() const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Save Game Subsystem")
	TArray<FSaveSlotMetadata> GetSaveSlotsMetadata() const;

	FActorSaveData GetPlayerSaveData() const;

protected:
	bool FindExistingSlotMetadata(const FString& SlotName, FSaveSlotMetadata*& OutSlotMetadata) const;

	void SerializeObject(UObject* Object, TArray<uint8>& OutResult);
	void DeserializeObject(const TArray<uint8>& Data, UObject* Object);

	void ExecuteOnPreSaveSafe(UObject* Target);
	void ExecuteOnSaveSafe(UObject* Target);
	void ExecuteOnPreLoadSafe(UObject* Target);
	void ExecuteOnLoadSafe(UObject* Target);

private:
	void PopulateCurrentSaveObject();
	void LoadActor(AActor* Actor, const FActorSaveData& ActorSaveData);
	FSaveSlotMetadata UpdateSlotMetadata(const FString& SlotName, const FString& DisplayName = "");

	FString SanitizeSlotName(const FString& SlotName);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSaveGame OnGameLoaded;

	UPROPERTY(BlueprintAssignable)
	FOnSaveGame OnGameSaved;

protected:
	// In this save object we save every game slot that has been created, so we can save info like Date or Play Time.
	UPROPERTY(Transient)
	TObjectPtr<UOvrlSaveGameSlots> SaveGameSlots;

	// Save game object for every game objects.
	UPROPERTY(Transient)
	TObjectPtr<UOvrlSaveGame> CurrentSaveGame;
};
