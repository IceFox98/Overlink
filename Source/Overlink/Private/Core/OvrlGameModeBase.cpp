// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OvrlGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "SaveSystem/OvrlSaveGameSubsystem.h"

void AOvrlGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// (Save/Load logic moved into new SaveGameSubsystem)
	UOvrlSaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<UOvrlSaveGameSubsystem>();

	// Optional slot name (Falls back to slot specified in SaveGameSettings class/INI otherwise)
	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	// SG->LoadSaveGame(SelectedSaveSlot);
}
