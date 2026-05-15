// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OvrlGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API AOvrlGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

};
