// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OvrlSaveGameSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Ovrl Save Game Settings"))
class OVERLINK_API UOvrlSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UOvrlSaveGameSettings();

public:
	// The name of the slot (.sav) where the game slot names are saved.
	// It's used to show the available game slot in load/save game UI page.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotNames;
	
	// Default slot name if any cached slot has been found.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString DefaultSaveSlotName;
};
