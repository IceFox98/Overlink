// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlGameConstants.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OVERLINK_API UOvrlGameConstants : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Game Constants", WorldContext = "WorldContextObject"))
	static const UOvrlGameConstants* Get(const UObject* WorldContextObject);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	TSoftObjectPtr<UWorld> MenuLevel;
	
	// Default level that will be loaded when a new game slot is created.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Gameplay")
	TSoftObjectPtr<UWorld> GameDefaultLevel;
};
