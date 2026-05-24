// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/OvrlGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "SaveSystem/OvrlSaveGameSettings.h"
#include "SaveSystem/OvrlSaveGameSubsystem.h"

void AOvrlGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();
	SG->LoadSelectedSlot();
}

APawn* AOvrlGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();
	const FActorSaveData PlayerSaveData = SG->GetPlayerSaveData();
	if (PlayerSaveData.IsValid())
	{
		// Override player pawn spawning to handle the deserialization of its data, before its BeginPlay.
		APawn* ResultPawn = GetWorld()->SpawnActorDeferred<APawn>(PlayerSaveData.ActorClass, PlayerSaveData.Transform, NewPlayer, GetInstigator());
		if (!ResultPawn)
		{
			UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PlayerSaveData.ActorClass), *PlayerSaveData.Transform.ToHumanReadableString());
			return nullptr;
		}
		
		// Load SaveGame properties
		SG->LoadPlayerData(ResultPawn);
		
		UGameplayStatics::FinishSpawningActor(ResultPawn, PlayerSaveData.Transform);
		return ResultPawn;
	}

	// If we don't have any player SaveData, let's run default behavior.
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

void AOvrlGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// This will call SpawnDefaultPawnFor_Implementation during the spawn of the player pawn.
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	// Here, the player pawn has been spawned and possessed.
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();
	SG->OnPostPlayerPossessed();
}
