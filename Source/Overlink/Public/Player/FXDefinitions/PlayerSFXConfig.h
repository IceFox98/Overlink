// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerSFXConfig.generated.h"

/**
 *
 */
UCLASS()
class OVERLINK_API UPlayerSFXConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TObjectPtr<USoundBase> WalkingSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TObjectPtr<USoundBase> RunningSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TObjectPtr<USoundBase> JumpSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TObjectPtr<USoundBase> LandingSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TObjectPtr<USoundBase> MantleSFX;
};
