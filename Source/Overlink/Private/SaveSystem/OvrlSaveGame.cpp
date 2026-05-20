// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGame.h"

void UOvrlSaveGame::Initialize(const FString& InSlotName)
{
	SlotName = InSlotName;

	// // Reset all data in case this slot has already be used before.
	// SavedPlayers.Empty();
	// SavedActorMap.Empty();
}
