// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGame.h"

#include "GameFramework/PlayerState.h"
#include "Logging/StructuredLog.h"

void UOvrlSaveGame::Initialize(const FString& InSlotName)
{
	SlotName = InSlotName;

	// // Reset all data in case this slot has already be used before.
	// SavedPlayers.Empty();
	// SavedActorMap.Empty();
}

FEntitySaveData* UOvrlSaveGame::GetPlayerData(APlayerState* InPlayerState)
{
	// check(PlayerState);
	//
	// // Will not give unique ID while PIE so we skip that step while testing in editor.
	// // UObjects don't have access to UWorld, so we grab it via PlayerState instead
	// if (PlayerState->GetWorld()->IsPlayInEditor())
	// {
	// 	UE_LOGFMT(LogTemp, Log, "During PIE we cannot use PlayerID to retrieve Saved Player data. Using first entry in array if available.");
	//
	// 	if (SavedPlayers.IsValidIndex(0))
	// 	{
	// 		return &SavedPlayers[0];
	// 	}
	//
	// 	// No saved player data available
	// 	return nullptr;
	// }
	//
	// // Easiest way to deal with the different IDs is as FString (original Steam id is uint64)
	// FString PlayerID = PlayerState->GetUniqueId().ToString();
	// // Iterate the array and match by PlayerID (eg. unique ID provided by Steam)
	// return SavedPlayers.FindByPredicate([&](const FPlayerSaveData& Data) { return Data.PlayerID == PlayerID; });
	
	return nullptr;
}
