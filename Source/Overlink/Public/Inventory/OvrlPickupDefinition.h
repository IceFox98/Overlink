// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlPickupDefinition.generated.h"

class AOvrlItemPickupActor;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API UOvrlPickupDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	// Base class used to spawn a Pickup Actor when this item is dropped.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Pickup Definition")
	TSubclassOf<AOvrlItemPickupActor> BasePickupClass;
	
	// Sets the scale of the item mesh in the target Pickup Actor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Pickup Info")
	FVector MeshScale = FVector::One();

	// Sets the transform of the pickup collider in the target Pickup Actor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Pickup Info")
	FTransform PickupColliderTransform;

	// Sets the half height of the capsule pickup collider in the target Pickup Actor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Pickup Info")
	float PickupColliderCapsuleHalfHeight = 20.f;

	// Sets the radius of the capsule pickup collider in the target Pickup Actor.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Pickup Info")
	float PickupColliderCapsuleRadius = 16.f;
};
