// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/OvrlGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Net/Iris/ReplicationSystem/ReplicationSystemUtil.h"
#include "SaveSystem/OvrlSaveGameSubsystem.h"

void AOvrlGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	// FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	SG->LoadGame("TestSave");
}

APawn* AOvrlGameModeBase::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();
	const FActorSaveData PlayerSaveData = SG->GetPlayerSaveData();
	if (PlayerSaveData.IsValid())
	{
		APawn* ResultPawn = GetWorld()->SpawnActorDeferred<APawn>(PlayerSaveData.ActorClass, PlayerSaveData.Transform, NewPlayer, GetInstigator());
		if (!ResultPawn)
		{
			UE_LOG(LogGameMode, Warning, TEXT("SpawnDefaultPawnAtTransform: Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(PlayerSaveData.ActorClass), *PlayerSaveData.Transform.ToHumanReadableString());
			return nullptr;
		}
		
		SG->LoadPlayerData(ResultPawn);
		
		UGameplayStatics::FinishSpawningActor(ResultPawn, PlayerSaveData.Transform);
		return ResultPawn;
	}

	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

void AOvrlGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();
	SG->OnPostPlayerPossesed();
}
