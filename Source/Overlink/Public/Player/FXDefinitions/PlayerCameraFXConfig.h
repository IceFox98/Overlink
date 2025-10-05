// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCameraFXConfig.generated.h"

class UCameraShakeBase;

/**
 *
 */
UCLASS()
class OVERLINK_API UPlayerCameraFXConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TSubclassOf<UCameraShakeBase> MantleFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TSubclassOf<UCameraShakeBase> LateralWallrunFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TSubclassOf<UCameraShakeBase> VerticalWallrunFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
		TSubclassOf<UCameraShakeBase> LandingFX;
};
