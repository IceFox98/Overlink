// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PickupDefinition.generated.h"

class UItemDefinition;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class ONEBULLETMAN_API UPickupDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//Defines the pickup's actors to spawn, abilities to grant, and tags to add
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "OBM|Pickup")
		TSubclassOf<UItemDefinition> ItemDefinition;

	//Visual representation of the pickup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Pickup|Mesh")
		UStaticMesh* DisplayMesh;

	//Sets the height of the display mesh above the Weapon spawner
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Pickup|Mesh")
		FVector MeshScale = FVector(1.0f, 1.0f, 1.0f);

	//Sound to play when picked up
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Pickup")
		USoundBase* PickedUpSound;
};
