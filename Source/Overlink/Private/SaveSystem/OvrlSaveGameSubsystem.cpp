// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGameSubsystem.h"

// Internal
#include "Core/Interfaces/OvrlSaveableObject.h"
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

// bool UOvrlSaveGameSubsystem::CreateSaveGameSlot(FString SlotName)
// {
// 	// Create save game object
// 	UOvrlSaveGame* NewSaveGameSlot = CastChecked<UOvrlSaveGame>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGame::StaticClass()));
// 	if (!NewSaveGameSlot)
// 	{
// 		OVRL_LOG_ERR(LogOverlink, true, "Failed to create Save Game Object!");
// 		return false;
// 	}
//
// 	OVRL_LOG_INFO(LogOverlink, true, "Created New SaveGame Data.");
//
// 	NewSaveGameSlot->Initialize(SlotName);
//
// 	UGameplayStatics::SaveGameToSlot(NewSaveGameSlot, SlotName, 0);
//
// 	return true;
// }

void UOvrlSaveGameSubsystem::SaveGame(FString SlotName)
{
	// UOvrlSaveGame* SaveGameObj = nullptr;
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

	FSaveSlotMetadata SlotMetadata;
	const bool bFound = FindExistingSlotMetadata(SlotName, SlotMetadata);
	SlotMetadata.SlotName = SlotName;
	SlotMetadata.Date = FDateTime::UtcNow();

	if (!bFound)
	{
		SaveGameSlots->SaveSlotMetas.Add(SlotMetadata);
	}

	// Write the data in the CurrentSaveGame object
	PopulateCurrentSaveObject();

	// Write save object on disk
	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSaveGame->SlotName, 0);

	OnGameSaved.Broadcast(CurrentSaveGame);
}

void UOvrlSaveGameSubsystem::LoadGame(FString SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		OVRL_LOG_WARN(LogOverlink, true, "Failed to load the game, SlotName does not exist! You have to create a new save slot first.");
		return;
	}

	CurrentSaveGame = Cast<UOvrlSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	
	UGameplayStatics::OpenLevel(this, *CurrentSaveGame->LevelName);
}

bool UOvrlSaveGameSubsystem::FindExistingSlotMetadata(FString SlotName, FSaveSlotMetadata& OutSlotMetadata)
{
	for (const FSaveSlotMetadata& SlotMetadata : SaveGameSlots->SaveSlotMetas)
	{
		if (SlotMetadata.SlotName.Equals(SlotName, ESearchCase::IgnoreCase))
		{
			OutSlotMetadata = SlotMetadata;
			return true;
		}
	}

	return false;
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

FString UOvrlSaveGameSubsystem::GetLastSaveSlotName() const
{
	if (SaveGameSlots->SaveSlotMetas.IsEmpty())
	{
		return {};
	}

	return SaveGameSlots->SaveSlotMetas.Last().SlotName;
}

FActorSaveData UOvrlSaveGameSubsystem::GetPlayerSaveData() const
{
	if (CurrentSaveGame)
	{
		return CurrentSaveGame->PlayerData;
	}

	return FActorSaveData();
}

void UOvrlSaveGameSubsystem::PopulateCurrentSaveObject()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	CurrentSaveGame->LevelName = GetWorld()->GetMapName();

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
