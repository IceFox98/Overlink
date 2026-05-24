// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGameSubsystem.h"

// Internal
#include "Core/Interfaces/OvrlSaveableObject.h"
#include "Core/OvrlGameConstants.h"
#include "SaveSystem/OvrlSaveGame.h"
#include "SaveSystem/OvrlSaveGameSettings.h"
#include "OvrlLogUtils.h"

// Engine
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void UOvrlSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UOvrlSaveGameSettings* SGSettings = GetDefault<UOvrlSaveGameSettings>();
	if (UGameplayStatics::DoesSaveGameExist(SGSettings->SaveSlotNames, 0))
	{
		SaveGameSlots = Cast<UOvrlSaveGameSlots>(UGameplayStatics::LoadGameFromSlot(SGSettings->SaveSlotNames, 0));
	}
	else
	{
		SaveGameSlots = CastChecked<UOvrlSaveGameSlots>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGameSlots::StaticClass()));
		UGameplayStatics::SaveGameToSlot(SaveGameSlots, SGSettings->SaveSlotNames, 0);
	}
}

void UOvrlSaveGameSubsystem::SerializeObject(UObject* Object, TArray<uint8>& OutResult)
{
	if (!Object)
	{
		OVRL_LOG_ERR(LogOverlink, false, "Source Object is NULL!");
		return;
	}

	FMemoryWriter MemoryWriter = FMemoryWriter(OutResult, true);
	FOvrlSaveGameArchive Archive = FOvrlSaveGameArchive(MemoryWriter);

	// Serialize all the UPROPERTY(SaveGame)
	Object->Serialize(Archive);
}

void UOvrlSaveGameSubsystem::DeserializeObject(const TArray<uint8>& Data, UObject* Object)
{
	if (!Object)
	{
		OVRL_LOG_ERR(LogOverlink, false, "Target Object is NULL!");
		return;
	}

	FMemoryReader MemoryReader(Data, true);
	FOvrlSaveGameArchive Archive(MemoryReader);

	// Read the SaveGame properties and load their data/value
	Object->Serialize(Archive);
}

void UOvrlSaveGameSubsystem::CreateNewSaveSlot(FString SlotDisplayName, bool bUseDefaultLevel, FSaveSlotMetadata& NewSlotMetadata, bool& bSuccess)
{
	const UOvrlGameConstants* GameConstants = UOvrlGameConstants::Get(this);
	if (!GameConstants->GameDefaultLevel.ToSoftObjectPath().IsValid())
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to create new save slot! DefaultLevel is NULL!");
		return;
	}

	FString SlotName = SanitizeSlotName(SlotDisplayName);
	if (SlotName.IsEmpty())
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to create new save slot! SlotName is empty!");
		return;
	}

	// Append timestamp so the name will be 100% unique.
	SlotName = FString::Format(TEXT("{0}_{1}"), { SlotName, FDateTime::Now().ToUnixTimestamp() });

	// Create save game object
	CurrentSaveGame = CastChecked<UOvrlSaveGame>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGame::StaticClass()));
	if (!CurrentSaveGame)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to create Save Game Object!");
		return;
	}

	CurrentSaveGame->Initialize(SlotName);
	SaveSlotMetadata(SlotName, SlotDisplayName);

	if (bUseDefaultLevel)
	{
		CurrentSaveGame->LevelName = GameConstants->GameDefaultLevel.ToSoftObjectPath().GetAssetName();
	}
	else
	{
		CurrentSaveGame->LevelName = UGameplayStatics::GetCurrentLevelName(this);
	}

	// Write new slot on disk
	bSuccess = UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
	if (!bSuccess)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to save file on disk!");
	}
}

void UOvrlSaveGameSubsystem::SaveGame(FString SlotName)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<UOvrlSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	}
	else
	{
		CurrentSaveGame = CastChecked<UOvrlSaveGame>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGame::StaticClass()));
		CurrentSaveGame->Initialize(SlotName);
	}

	if (!CurrentSaveGame)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to save the game! CurrentSaveGame is not valid.");
		return;
	}

	SaveSlotMetadata(SlotName);

	// Write the data in the CurrentSaveGame object
	PopulateCurrentSaveObject();

	// Write save object on disk
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSaveGame->SlotName, 0);

	OnGameSaved.Broadcast(CurrentSaveGame);

	OVRL_LOG_INFO(LogOverlink, true, "Game Saved!");
}

void UOvrlSaveGameSubsystem::SaveCurrentSlot()
{
	if (CurrentSaveGame)
	{
		SaveGame(CurrentSaveGame->SlotName);
	}
}

void UOvrlSaveGameSubsystem::LoadGame(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		OVRL_LOG_WARN(LogOverlink, true, "Failed to load the game, SlotName does not exist! You have to create a new save slot first.");
		return;
	}

	CurrentSaveGame = Cast<UOvrlSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	UGameplayStatics::OpenLevel(this, *CurrentSaveGame->LevelName);
}

bool UOvrlSaveGameSubsystem::DeleteSaveSlot(const FString& SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		OVRL_LOG_WARN(LogOverlink, true, "Failed to load the game, SlotName does not exist! You have to create a new save slot first.");
		return false;
	}

	// Remove slot metadata
	for (auto It = SaveGameSlots->SaveSlotMetas.CreateIterator(); It; ++It)
	{
		FSaveSlotMetadata& SlotMetadata = *It;
		if (SlotMetadata.SlotName == SlotName)
		{
			It.RemoveCurrent();
			break;
		}
	}

	SaveGameSlots->LastPlayerSlotIndex = INDEX_NONE;
	SaveSlotNames();

	// Delete the actual save game slot
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);

	CurrentSaveGame = nullptr;
	
	OVRL_LOG_INFO(LogOverlink, true, "Slot '%s' deleted successfully!", *SlotName);
	
	return true;
}

void UOvrlSaveGameSubsystem::LoadPlayerData(APawn* Player)
{
	// Load player character data.
	// Here, the player has still not finish to spawn, so we can load the data BEFORE the BeginPlay and components initialization.
	const FActorSaveData PlayerSaveData = GetPlayerSaveData();
	if (PlayerSaveData.IsValid())
	{
		LoadActor(Player, PlayerSaveData);
	}
}

void UOvrlSaveGameSubsystem::OnPostPlayerPossessed()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	// Here we have valid Player Controller and other stuff
	if (AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld()))
	{
		ExecuteOnPreLoadSafe(GS);
		DeserializeObject(CurrentSaveGame->GameState, GS);
		ExecuteOnLoadSafe(GS);
	}

	if (APlayerState* PS = UGameplayStatics::GetPlayerState(GetWorld(), 0))
	{
		ExecuteOnPreLoadSafe(PS);
		DeserializeObject(CurrentSaveGame->PlayerState, PS);
		ExecuteOnLoadSafe(PS);
	}

	// Rotate the player towards its last saved look direction
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		ExecuteOnPreLoadSafe(PC);
		PC->SetControlRotation(CurrentSaveGame->ControlRotation);
		DeserializeObject(CurrentSaveGame->PlayerController, PC);
		ExecuteOnLoadSafe(PC);
	}

	OnGameLoaded.Broadcast(CurrentSaveGame);
}

void UOvrlSaveGameSubsystem::PopulateCurrentSaveObject()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->Reset();

	CurrentSaveGame->LevelName = UGameplayStatics::GetCurrentLevelName(this);

	// Save game framework related objects
	if (AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld()))
	{
		ExecuteOnPreSaveSafe(GS);
		SerializeObject(GS, CurrentSaveGame->GameState);
		ExecuteOnSaveSafe(GS);
	}

	if (APlayerState* PS = UGameplayStatics::GetPlayerState(GetWorld(), 0))
	{
		ExecuteOnPreSaveSafe(PS);
		SerializeObject(PS, CurrentSaveGame->PlayerState);
		ExecuteOnSaveSafe(PS);
	}

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		ExecuteOnPreSaveSafe(PC);
		SerializeObject(PC, CurrentSaveGame->PlayerController);
		CurrentSaveGame->ControlRotation = PC->GetControlRotation();
		ExecuteOnSaveSafe(PC);
	}

	// Save only Actors with Saveable Object interface
	TArray<AActor*> SaveableActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UOvrlSaveableObject::StaticClass(), SaveableActors);

	for (AActor* Actor : SaveableActors)
	{
		if (!Actor)
		{
			// Should not be possible, but never say never.
			continue;
		}

		if (Actor->IsPendingKillPending())
		{
			// This is Actor is not valid anymore, skip it's save
			continue;
		}

		// Notify Actor that we're going to save its data
		IOvrlSaveableObject::Execute_OnPreSave(Actor);

		FActorSaveData ActorData;
		ActorData.Name = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();
		ActorData.ActorClass = Actor->GetClass();
		const bool bWasSpawned = !Actor->IsNetStartupActor();

		// Serialize all its SaveGame properties
		SerializeObject(Actor, ActorData.ByteData);

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		// Serialize Actor's component too
		for (UActorComponent* Component : Components)
		{
			FComponentSaveData ComponentSaveData;
			ComponentSaveData.Name = Component->GetFName();
			ComponentSaveData.ComponentClass = Component->GetClass();

			if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
			{
				ComponentSaveData.RelativeTransform = SceneComponent->GetRelativeTransform();
			}

			// Serialize component data 
			ExecuteOnPreSaveSafe(Component);
			SerializeObject(Component, ComponentSaveData.ByteData);
			ExecuteOnSaveSafe(Component);

			ActorData.ComponentsSaveData.Add(ComponentSaveData);
		}

		// We treat Player as a special Actor since it's spawned by the GameMode
		const bool bIsPlayer = IOvrlSaveableObject::Execute_SaveAsPlayer(Actor);

		if (bIsPlayer) // We found the player!
		{
			if (ensureMsgf(!CurrentSaveGame->PlayerData.IsValid(),
				TEXT("PlayerData has already been saved once! For now, only 1 PlayerData can be saved per slot."
					"Ensure that only one Actor (player pawn) in the level returns 'true' from the SaveAsPlayer() interface function.")))
			{
				CurrentSaveGame->PlayerData = ActorData;
			}
		}
		else if (bWasSpawned)
		{
			// Save runtime Actors so that we can re-spawn them on game load.
			CurrentSaveGame->SpawnedActors.Add(ActorData);
		}
		else
		{
			// Save level Actors in case they have been moved or changed.
			CurrentSaveGame->LevelActors.Add(ActorData.Name, ActorData);
		}

		// Notify Actor that we're done saving its data
		IOvrlSaveableObject::Execute_OnSave(Actor);
	}
}

void UOvrlSaveGameSubsystem::LoadSelectedSlot()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	if (CurrentSaveGame->LevelActors.IsEmpty() && CurrentSaveGame->SpawnedActors.IsEmpty())
	{
		// No data to load, skip.
		return;
	}

	// NOTE: This code is executed BEFORE the BeginPlay of any Actor (even the runtime-spawned ones).
	// This is useful because when BeginPlay is called, you already have the Actor properties loaded in memory, and you can
	// easily handle different logic depending on that data.

	TArray<AActor*> SaveableActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UOvrlSaveableObject::StaticClass(), SaveableActors);

	// Loop through level actors
	for (AActor* Actor : SaveableActors)
	{
		if (!Actor)
		{
			continue;
		}

		// Check if the current level Actor has any Save Data
		FActorSaveData* LevelActorSaveData = CurrentSaveGame->LevelActors.Find(Actor->GetFName());

		if (LevelActorSaveData)
		{
			// If it exists, load its properties
			LoadActor(Actor, *LevelActorSaveData);
		}
		else
		{
			// If we found a Saveable Actor placed in the level, but without Save Data, it means it was destroyed during gameplay.
			Actor->Destroy();
		}
	}

	// Re-spawn the actors spawned during gameplay
	for (const FActorSaveData& SpawnedActorSaveData : CurrentSaveGame->SpawnedActors)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bDeferConstruction = true; // Manually set deferred spawn so we can set the Name
		SpawnInfo.Name = SpawnedActorSaveData.Name;

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SpawnedActorSaveData.ActorClass, SpawnedActorSaveData.Transform, SpawnInfo);
		if (SpawnedActor)
		{
			LoadActor(SpawnedActor, SpawnedActorSaveData);

			UGameplayStatics::FinishSpawningActor(SpawnedActor, SpawnedActorSaveData.Transform);
		}
	}
}

void UOvrlSaveGameSubsystem::LoadActor(AActor* Actor, const FActorSaveData& ActorSaveData)
{
	if (!Actor)
	{
		return;
	}

	// Notify Actor that we're going to load its data
	ExecuteOnPreLoadSafe(Actor);

	Actor->SetActorTransform(ActorSaveData.Transform);
	DeserializeObject(ActorSaveData.ByteData, Actor);

	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);

	// Load Actor's component data
	for (const FComponentSaveData& ComponentSaveData : ActorSaveData.ComponentsSaveData)
	{
		for (UActorComponent* Component : Components)
		{
			if (ComponentSaveData.Name == Component->GetFName())
			{
				ExecuteOnPreLoadSafe(Component);

				if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
				{
					SceneComponent->SetRelativeTransform(ComponentSaveData.RelativeTransform);
				}

				DeserializeObject(ComponentSaveData.ByteData, Component);

				ExecuteOnLoadSafe(Component);
			}
		}
	}

	// Notify Actor that we're done loading its data
	ExecuteOnLoadSafe(Actor);
}

TArray<FSaveSlotMetadata> UOvrlSaveGameSubsystem::GetSaveSlotsMetadata() const
{
	return SaveGameSlots->SaveSlotMetas;
}

FSaveSlotMetadata UOvrlSaveGameSubsystem::GetMostRecentSaveSlotMetadata() const
{
	if (SaveGameSlots->SaveSlotMetas.IsValidIndex(SaveGameSlots->LastPlayerSlotIndex))
	{
		return SaveGameSlots->SaveSlotMetas[SaveGameSlots->LastPlayerSlotIndex];
	}
	
	return FSaveSlotMetadata();
}

FActorSaveData UOvrlSaveGameSubsystem::GetPlayerSaveData() const
{
	if (CurrentSaveGame)
	{
		return CurrentSaveGame->PlayerData;
	}

	return FActorSaveData();
}

void UOvrlSaveGameSubsystem::SaveSlotMetadata(const FString& SlotName, const FString& DisplayName/* = ""*/)
{
	FSaveSlotMetadata* SlotMetadata = new FSaveSlotMetadata();
	const bool bFound = FindExistingSlotMetadata(SlotName, SlotMetadata);

	if (!SlotMetadata)
	{
		return;
	}

	SlotMetadata->SlotName = SlotName;
	SlotMetadata->Date = FDateTime::Now();
	if (!DisplayName.IsEmpty())
	{
		SlotMetadata->DisplayName = DisplayName;
	}

	if (!bFound)
	{
		const int32 Index = SaveGameSlots->SaveSlotMetas.Add(*SlotMetadata);
		SaveGameSlots->SaveSlotMetas[Index].SlotIndex = Index;
	}
	
	SaveGameSlots->LastPlayerSlotIndex = SlotMetadata->SlotIndex;
	
	SaveSlotNames();
}

bool UOvrlSaveGameSubsystem::FindExistingSlotMetadata(const FString& SlotName, FSaveSlotMetadata*& OutSlotMetadata) const
{
	for (FSaveSlotMetadata& SlotMetadata : SaveGameSlots->SaveSlotMetas)
	{
		if (SlotMetadata.SlotName.Equals(SlotName, ESearchCase::IgnoreCase))
		{
			OutSlotMetadata = &SlotMetadata;
			return true;
		}
	}

	return false;
}

void UOvrlSaveGameSubsystem::SaveSlotNames() const
{
	const UOvrlSaveGameSettings* SGSettings = GetDefault<UOvrlSaveGameSettings>();
	UGameplayStatics::SaveGameToSlot(SaveGameSlots, SGSettings->SaveSlotNames, 0);
}

FString UOvrlSaveGameSubsystem::SanitizeSlotName(const FString& SlotName)
{
	FString FinalName = FPaths::MakeValidFileName(SlotName);
	FinalName.TrimStartAndEndInline();
	FinalName.ReplaceInline(TEXT(" "), TEXT(""));
	return FinalName;
}

void UOvrlSaveGameSubsystem::ExecuteOnPreSaveSafe(UObject* Target)
{
	if (Target && Target->Implements<UOvrlSaveableObject>())
	{
		IOvrlSaveableObject::Execute_OnPreSave(Target);
	}
}

void UOvrlSaveGameSubsystem::ExecuteOnSaveSafe(UObject* Target)
{
	if (Target && Target->Implements<UOvrlSaveableObject>())
	{
		IOvrlSaveableObject::Execute_OnSave(Target);
	}
}

void UOvrlSaveGameSubsystem::ExecuteOnPreLoadSafe(UObject* Target)
{
	if (Target && Target->Implements<UOvrlSaveableObject>())
	{
		IOvrlSaveableObject::Execute_OnPreLoad(Target);
	}
}

void UOvrlSaveGameSubsystem::ExecuteOnLoadSafe(UObject* Target)
{
	if (Target && Target->Implements<UOvrlSaveableObject>())
	{
		IOvrlSaveableObject::Execute_OnLoad(Target);
	}
}
