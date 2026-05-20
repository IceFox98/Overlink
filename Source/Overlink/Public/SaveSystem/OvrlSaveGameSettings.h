// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OvrlSaveGameSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Save Game Settings")) // 'DefaultConfig' = "Save object config only to Default INIs, never to local INIs."
class OVERLINK_API UOvrlSaveGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UOvrlSaveGameSettings();

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString SaveSlotNames;
	
	// Default slot name if any cached slot has been found.
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "General")
	FString DefaultSaveSlotName;
};
