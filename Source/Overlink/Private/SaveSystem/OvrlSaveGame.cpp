// Fill out your copyright notice in the Description page of Project Settings.

#include "SaveSystem/OvrlSaveGame.h"

void UOvrlSaveGame::Initialize(const FString& InSlotName)
{
	SlotName = InSlotName;
}

void UOvrlSaveGame::Reset()
{
	PlayerData = {};
	LevelActors.Empty();
	SpawnedActors.Empty();
}
