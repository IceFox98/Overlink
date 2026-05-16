// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGameSubsystem.h"

// Internal
#include "OvrlLogUtils.h"
#include "Core/Interfaces/OvrlSaveableObject.h"
#include "SaveSystem/OvrlSaveGame.h"
#include "SaveSystem/OvrlSaveGameSettings.h"
#include "Inventory/OvrlInventoryComponent.h"

// Engine
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void UOvrlSaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UOvrlSaveGameSettings* SGSettings = GetDefault<UOvrlSaveGameSettings>();
	// Access defaults from DefaultGame.ini
	CurrentSlotName = SGSettings->SaveSlotName;
}

void UOvrlSaveGameSubsystem::SerializeObject(UObject* Object, TArray<uint8>& OutResult)
{
	if (!Object)
	{
		return;
	}

	FMemoryWriter MemoryWriter = FMemoryWriter(OutResult, true);
	FOvrlSaveGameArchive Archive = FOvrlSaveGameArchive(MemoryWriter);

	Object->Serialize(Archive);
}

void UOvrlSaveGameSubsystem::DeserializeObject(const TArray<uint8>& Data, UObject* Object)
{
	if (!Object)
	{
		return;
	}

	FMemoryReader MemoryReader(Data, true);
	FOvrlSaveGameArchive Archive(MemoryReader);

	Object->Serialize(Archive);
}

void UOvrlSaveGameSubsystem::CreateNewSaveGame(FString SlotName)
{
	CurrentSaveGame = CastChecked<UOvrlSaveGame>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGame::StaticClass()));
	if (!CurrentSaveGame)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to create Save Game Object!");
		return;
	}

	OVRL_LOG_INFO(LogOverlink, true, "Created New SaveGame Data.");

	CurrentSaveGame->Initialize(SlotName);
	SaveCurrentSlot();
}

void UOvrlSaveGameSubsystem::SaveCurrentSlot()
{
	if (!CurrentSaveGame)
	{
		OVRL_LOG_WARN(LogOverlink, true, "Failed to save the current slot!");
		return;
	}

	PopulateCurrentSlot();

	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSaveGame->SlotName, 0);

}

void UOvrlSaveGameSubsystem::LoadGame(FString SlotName)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		return;
	}

	CurrentSaveGame = Cast<UOvrlSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	PopulateFromCurrentSlot();
}

void UOvrlSaveGameSubsystem::LoadPlayerData(APawn* Player)
{
	// Load player character data
	// Here, the player has still not finish to spawn, so we can load the data BEFORE the BeginPlay and components initialization
	const FActorSaveData PlayerSaveData = GetPlayerSaveData();
	if (PlayerSaveData.IsValid())
	{
		LoadActor(Player, PlayerSaveData);
	}
}

void UOvrlSaveGameSubsystem::OnPostPlayerPossesed()
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
}

bool UOvrlSaveGameSubsystem::OverrideSpawnTransform(AController* NewPlayer)
{
	// check(NewPlayer);
	//
	// APlayerState* PS = NewPlayer->GetPlayerState<APlayerState>();
	// check(PS);
	//
	// if (APawn* MyPawn = PS->GetPawn())
	// {
	// 	FPlayerSaveData* FoundData = CurrentSaveGame->GetPlayerData(PS);
	// 	if (FoundData)
	// 	{
	// 		MyPawn->SetActorLocation(FoundData->Location);
	// 		MyPawn->SetActorRotation(FoundData->Rotation);
	//
	// 		// PlayerState owner is a (Player)Controller
	// 		AController* PC = CastChecked<AController>(PS->GetOwner());
	// 		// Set control rotation to change camera direction, setting Pawn rotation is not enough
	// 		PC->SetControlRotation(FoundData->Rotation);
	//
	// 		return true;
	// 	}
	// }

	return false;
}

void UOvrlSaveGameSubsystem::SetSlotName(FString NewSlotName)
{
	if (NewSlotName.IsEmpty())
	{
		return;
	}

	CurrentSlotName = NewSlotName;
}

FActorSaveData UOvrlSaveGameSubsystem::GetPlayerSaveData() const
{
	if (CurrentSaveGame)
	{
		return CurrentSaveGame->PlayerData;
	}

	return FActorSaveData();
}

//
// void UOvrlSaveGameSubsystem::WriteSaveGame()
// {
// 	// Clear arrays, may contain data from previously loaded SaveGame
// 	CurrentSaveGame->SavedPlayers.Empty();
// 	CurrentSaveGame->SavedActorMap.Empty();
//
// 	AGameStateBase* GS = GetWorld()->GetGameState();
// 	check(GS);
//
// 	// // Iterate all player states, we don't have proper ID to match yet (requires Steam or EOS)
// 	// for (int32 i = 0; i < GS->PlayerArray.Num(); i++)
// 	// {
// 	// 	AOvrlPlayerState* PS = CastChecked<AOvrlPlayerState>(GS->PlayerArray[i]);
// 	// 	if (PS)
// 	// 	{
// 	// 		PS->SavePlayerState(CurrentSaveGame);
// 	// 		break; // single player only supported at this point
// 	// 	}
// 	// }
//
// 	// Iterate the entire world of actors
// 	for (AActor* Actor : TActorRange<AActor>(GetWorld()))
// 	{
// 		// Only interested in our 'gameplay actors', skip actors that are being destroyed
// 		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
// 		if (!IsValid(Actor) || !Actor->Implements<UOvrlSaveableObject>())
// 		{
// 			continue;
// 		}
//
// 		// Actor->IsNetStartupActor();
//
// 		FActorSaveData ActorData;
// 		ActorData.ActorName = Actor->GetFName();
// 		ActorData.Transform = Actor->GetActorTransform();
// 		ActorData.ActorClass = Actor->GetClass();
// 		ActorData.bWasSpawned = !Actor->IsNetStartupActor();
//
// 		{
// 			// Pass the array to fill with data from Actor
// 			FMemoryWriter MemWriter(ActorData.ByteData);
//
// 			FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
// 			// Find only variables with UPROPERTY(SaveGame)
// 			Ar.ArIsSaveGame = true;
// 			// Converts Actor's SaveGame UPROPERTIES into binary array
// 			Actor->Serialize(Ar);
// 		}
//
// 		// TArray<UActorComponent*> Components;
// 		// Actor->GetComponents(Components);
// 		//
// 		// for (UActorComponent* Comp : Components)
// 		// {
// 		// 	if (Comp->Implements<UOvrlSaveableObject>())
// 		// 	{
// 		// 		// Pass the array to fill with data from Actor
// 		// 		FMemoryWriter MemWriter(ActorData.ByteData);
// 		//
// 		// 		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
// 		// 		// Find only variables with UPROPERTY(SaveGame)
// 		// 		Ar.ArIsSaveGame = true;
// 		// 		// Converts Actor's SaveGame UPROPERTIES into binary array
// 		// 		Comp->Serialize(Ar);
// 		// 	}
// 		// }
//
// 		CurrentSaveGame->SavedActorMap.Add(Actor->GetFName(), ActorData);
// 	}
//
// 	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, CurrentSlotName, 0);
//
// 	OnSaveGameWritten.Broadcast(CurrentSaveGame);
// }
//
// void UOvrlSaveGameSubsystem::LoadSaveGame(FString InSlotName)
// {
// 	// Update slot name first if specified, otherwise keeps default name
// 	SetSlotName(InSlotName);
//
// 	if (UGameplayStatics::DoesSaveGameExist(CurrentSlotName, 0))
// 	{
// 		CurrentSaveGame = Cast<UOvrlSaveGame>(UGameplayStatics::LoadGameFromSlot(CurrentSlotName, 0));
// 		if (CurrentSaveGame == nullptr)
// 		{
// 			OVRL_LOG_WARN(LogOverlink, true, "Failed to load SaveGame Data.");
// 			return;
// 		}
//
// 		OVRL_LOG_INFO(LogOverlink, true, "Loaded SaveGame Data.");
//
// 		// Iterate the entire world of actors
// 		for (AActor* Actor : TActorRange<AActor>(GetWorld()))
// 		{
// 			// Only interested in our 'gameplay actors'
// 			if (!Actor->Implements<UOvrlSaveableObject>())
// 			{
// 				continue;
// 			}
//
// 			if (FActorSaveData* FoundData = CurrentSaveGame->SavedActorMap.Find(Actor->GetFName()))
// 			{
// 				Actor->SetActorTransform(FoundData->Transform);
//
// 				{
// 					FMemoryReader MemReader(FoundData->ByteData);
//
// 					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
// 					Ar.ArIsSaveGame = true;
// 					// Convert binary array back into actor's variables
// 					Actor->Serialize(Ar);
// 				}
//
// 				// TArray<UActorComponent*> Components;
// 				// Actor->GetComponents(Components);
// 				//
// 				// for (UActorComponent* Comp : Components)
// 				// {
// 				// 	if (Comp->Implements<UOvrlSaveableObject>())
// 				// 	{
// 				// 		// Pass the array to fill with data from Actor
// 				// 		FMemoryWriter MemWriter(ActorData.ByteData);
// 				//
// 				// 		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
// 				// 		// Find only variables with UPROPERTY(SaveGame)
// 				// 		Ar.ArIsSaveGame = true;
// 				// 		// Converts Actor's SaveGame UPROPERTIES into binary array
// 				// 		Comp->Serialize(Ar);
// 				// 	}
// 				// }
//
// 				// UOvrlSaveableObject::Execute_OnActorLoaded(Actor);
// 			}
// 		}
//
// 		// // Check for runtime-spawned actors
// 		// for (const TPair Pair : CurrentSaveGame->SavedActorMap)
// 		// {
// 		// 	const FActorSaveData ActorSaveData = Pair.Value;
// 		// 	
// 		// 	if (ActorSaveData.bWasSpawned)
// 		// 	{
// 		// 		AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorSaveData.ActorClass);
// 		// 		if (Actor)
// 		// 		{
// 		// 			Actor->SetActorTransform(ActorSaveData.Transform);
// 		//
// 		// 			FMemoryReader MemReader(ActorSaveData.ByteData);
// 		//
// 		// 			FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
// 		// 			Ar.ArIsSaveGame = true;
// 		// 			// Convert binary array back into actor's variables
// 		// 			Actor->Serialize(Ar);
// 		//
// 		// 			IOvrlSaveableActor::Execute_OnActorLoaded(Actor);
// 		// 		}
// 		// 	}
// 		// }
//
// 		OnSaveGameLoaded.Broadcast(CurrentSaveGame);
// 	}
// 	else
// 	{
// 		// CurrentSaveGame = CastChecked<UOvrlSaveGame>(UGameplayStatics::CreateSaveGameObject(UOvrlSaveGame::StaticClass()));
// 		// if (!CurrentSaveGame)
// 		// {
// 		// 	OVRL_LOG_ERR(LogOverlink, true, "Failed to create Save Game Object!");
// 		// 	return;
// 		// }
// 		//
// 		// CurrentSaveGame->Initialize(InSlotName);
// 		//
// 		// OVRL_LOG_INFO(LogOverlink, true, "Created New SaveGame Data.");
// 	}
// }

void UOvrlSaveGameSubsystem::PopulateCurrentSlot()
{
	if (!CurrentSaveGame)
	{
		return;
	}

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

	// ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	// if (Character)
	// {
	// 	FPlayerSaveData PlayerSaveData;
	// 	PlayerSaveData.PlayerTransform = Character->GetActorTransform();
	// 	SerializeObject(Character, CurrentSaveGame->PlayerCharacter);
	//
	// 	if (UOvrlInventoryComponent* InventoryComponent = Character->GetComponentByClass<UOvrlInventoryComponent>())
	// 	{
	// 		for (const FOvrlItemEntry& ItemEntry : InventoryComponent->GetItemEntries())
	// 		{
	// 			FInventoryItemEntrySaveData SaveData;
	// 			SaveData.ItemDefinition = ItemEntry.Instance->GetItemDefClass();
	// 			SaveData.Stacks = ItemEntry.Instance->Stacks;
	// 			SaveData.Quantity = ItemEntry.Quantity;
	// 			PlayerSaveData.Inventory.Add(SaveData);
	// 		}
	//
	// 		CurrentSaveGame->SavedPlayers.Add(PlayerSaveData);
	// 	}
	// }

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

		IOvrlSaveableObject::Execute_OnPreSave(Actor);

		FActorSaveData ActorData;
		ActorData.Name = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();
		ActorData.ActorClass = Actor->GetClass();
		ActorData.Outer = Actor->GetOuter();
		const bool bWasSpawned = !Actor->IsNetStartupActor();
		SerializeObject(Actor, ActorData.ByteData);

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);

		for (UActorComponent* Component : Components)
		{
			FComponentSaveData ComponentSaveData;
			ComponentSaveData.Name = Component->GetFName();
			ComponentSaveData.ComponentClass = Component->GetClass();

			if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
			{
				ComponentSaveData.RelativeTransform = SceneComponent->GetRelativeTransform();
			}

			ExecuteOnPreSaveSafe(Component);
			SerializeObject(Component, ComponentSaveData.ByteData);
			ExecuteOnSaveSafe(Component);

			ActorData.ComponentsSaveData.Add(ComponentSaveData);
		}

		const bool bIsPlayer = IOvrlSaveableObject::Execute_SaveAsPlayer(Actor);

		if (bIsPlayer) // We found the player!
		{
			if (ensureMsgf(!CurrentSaveGame->PlayerData.IsValid(),
				TEXT("PlayerData has already been saved once! For now, only 1 PlayerData can be saved per slot."
					"Ensure that only one Actor in the level returns 'true' from the SaveAsPlayer() interface function.")))
			{
				CurrentSaveGame->PlayerData = ActorData;
			}
		}
		else if (bWasSpawned)
		{
			CurrentSaveGame->SpawnedActors.Add(ActorData);
		}
		else
		{
			CurrentSaveGame->LevelActors.Add(ActorData.Name, ActorData);
		}

		IOvrlSaveableObject::Execute_OnSave(Actor);
	}
}

FEntitySaveData UOvrlSaveGameSubsystem::TryCreateEntitySaveData(const AActor* Actor, const FActorSaveData& ActorSaveData)
{
	if (!Actor)
	{
		return FEntitySaveData();
	}

	const UOvrlInventoryComponent* InventoryComponent = Actor->GetComponentByClass<UOvrlInventoryComponent>();

	// If no one of these components are valid, then it's not considered as an entity 
	if (!InventoryComponent)
	{
		return FEntitySaveData();
	}

	FEntitySaveData PlayerSaveData = ActorSaveData;

	if (InventoryComponent)
	{
		for (const FOvrlItemEntry& ItemEntry : InventoryComponent->GetItemEntries())
		{
			FInventoryItemEntrySaveData SaveData;
			SaveData.ItemDefinition = ItemEntry.Instance->GetItemDefClass();
			// SaveData.Stacks = ItemEntry.Instance->Stacks;
			SaveData.Quantity = ItemEntry.Quantity;
			PlayerSaveData.InventoryEntries.Add(SaveData);
		}
	}

	return PlayerSaveData;
}

void UOvrlSaveGameSubsystem::PopulateFromCurrentSlot()
{
	if (!CurrentSaveGame)
	{
		return;
	}

	// ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	// if (Character)
	// {
	// 	DeserializeObject(CurrentSaveGame->PlayerCharacter, Character);
	//
	// 	if (!CurrentSaveGame->SavedPlayers.IsValidIndex(0))
	// 	{
	// 		return;
	// 	}
	//
	// 	Character->SetActorTransform(CurrentSaveGame->SavedPlayers[0].PlayerTransform);
	//
	// 	if (CurrentSaveGame->SavedPlayers[0].Inventory.Num() > 0)
	// 	{
	// 		if (UOvrlInventoryComponent* InventoryComponent = Character->GetComponentByClass<UOvrlInventoryComponent>())
	// 		{
	// 			for (FInventoryItemEntrySaveData SaveData : CurrentSaveGame->SavedPlayers[0].Inventory)
	// 			{
	// 				UOvrlItemInstance* ItemInstance = InventoryComponent->CreateUniqueItem(SaveData.ItemDefinition);
	// 				if (ItemInstance)
	// 				{
	// 					ItemInstance->Stacks = SaveData.Stacks; // Override any existing item stacks
	// 					InventoryComponent->AddItem(ItemInstance);
	// 				}
	//
	// 				// FInventoryEntry NewEntry;
	// 				// NewEntry.Instance = NewObject<UInventoryItemInstance>();
	// 				// NewEntry.Instance->SetItemDefinition(Entry.ItemDefinition);
	// 				// NewEntry.Count = Entry.Count;
	// 				//
	// 				// InventoryComponent->AddItem(NewEntry);
	// 			}
	// 		}
	// 	}
	// }

	TArray<AActor*> SaveableActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UOvrlSaveableObject::StaticClass(), SaveableActors);

	for (AActor* Actor : SaveableActors)
	{
		if (!Actor)
		{
			continue;
		}

		// Check if the actor exists in the world objects on the save game object.
		FActorSaveData* LevelActorSaveData = CurrentSaveGame->LevelActors.Find(Actor->GetFName());

		// If a matching record has been found.
		if (LevelActorSaveData)
		{
			LoadActor(Actor, *LevelActorSaveData);
		}
		else
		{
			// If we found a Saveable Actor placed in the level, but without Save Data, it means it was destroyed during gameplay.
			Actor->Destroy();
		}

		// FEntitySaveData* EntitySaveData = CurrentSaveGame->EntitiesSaveData.FindByPredicate([Actor](const FEntitySaveData& EntitySaveData) {
		// 		return EntitySaveData.Name == Actor->GetFName();
		// 	}
		// );

		// if (EntitySaveData)
		// {
		// 	IOvrlSaveableObject::Execute_OnPreLoad(Actor);
		//
		// 	Actor->SetActorTransform(EntitySaveData->Transform);
		// 	DeserializeObject(EntitySaveData->ByteData, Actor);
		//
		// 	TArray<UActorComponent*> Components;
		// 	Actor->GetComponents(Components);
		//
		// 	for (const FComponentSaveData& ComponentSaveData : EntitySaveData->ComponentsSaveData)
		// 	{
		// 		for (UActorComponent* Component : Components)
		// 		{
		// 			if (ComponentSaveData.Name == Component->GetFName())
		// 			{
		// 				ExecuteOnPreLoadSafe(Component);
		//
		// 				if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
		// 				{
		// 					SceneComponent->SetWorldTransform(ComponentSaveData.Transform);
		// 				}
		//
		// 				if (UOvrlInventoryComponent* InventoryComponent = Cast<UOvrlInventoryComponent>(Component))
		// 				{
		// 					for (FInventoryItemEntrySaveData InventoryEntry : EntitySaveData->InventoryEntries)
		// 					{
		// 						UOvrlItemInstance* ItemInstance = InventoryComponent->CreateUniqueItem(InventoryEntry.ItemDefinition);
		// 						if (ItemInstance)
		// 						{
		// 							// ItemInstance->Stacks = InventoryEntry.Stacks; // Override any existing item stacks
		// 							InventoryComponent->AddItem(ItemInstance);
		// 						}
		// 					}
		//
		// 					DeserializeObject(ComponentSaveData.ByteData, Component);
		//
		// 					ExecuteOnLoadSafe(Component);
		// 				}
		// 			}
		// 		}
		// 	}
		//
		// 	IOvrlSaveableObject::Execute_OnLoad(Actor);
		// 	
		// 	// // If we couldn't find the world object, check if the world object is considered a "unique" collectible and has already been collected.
		// 	// if (CurrentSaveGame->UniqueCollectiblesCollected.Contains(Actor->GetName()))
		// 	// {
		// 	// 	// Despawn actor as the player should not be able to collect this collectible again.
		// 	// 	Actor->Destroy();
		// 	// 	continue;
		// 	// }
		// }
	}

	for (const FActorSaveData& SpawnedActorSaveData : CurrentSaveGame->SpawnedActors)
	{
		AActor* SpawnedActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnedActorSaveData.ActorClass, SpawnedActorSaveData.Transform);
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

	ExecuteOnPreLoadSafe(Actor);

	Actor->SetActorTransform(ActorSaveData.Transform);
	DeserializeObject(ActorSaveData.ByteData, Actor);

	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);

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
