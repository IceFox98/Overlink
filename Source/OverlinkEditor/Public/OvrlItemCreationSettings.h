// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "OvrlItemCreationSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta = (DisplayName="Ovrl Item Creation Settings"))
class OVERLINKEDITOR_API UOvrlItemCreationSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Prefixes")
	FString BlueprintPrefix = "BP_";
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Prefixes")
	FString ItemDefinitionPrefix = "ID_";
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Prefixes")
	FString EquipmentDefinitionPrefix = "ED_";
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = "Prefixes")
	FString PickupDefinitionPrefix = "PickupDefinition_";
};
