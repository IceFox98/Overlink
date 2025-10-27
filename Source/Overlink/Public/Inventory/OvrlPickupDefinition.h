// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlPickupDefinition.generated.h"

class UOvrlItemDefinition;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API UOvrlPickupDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ovrl|Pickup")
		TSubclassOf<UOvrlItemDefinition> ItemDefinition;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Pickup|Mesh")
		FVector MeshScale = FVector(1.0f, 1.0f, 1.0f);

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Pickup")
		USoundBase* PickedUpSound;
};
