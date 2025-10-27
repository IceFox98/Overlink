// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OvrlItemPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UOvrlPickupDefinition;
class UOvrlItemInstance;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API AOvrlItemPickupActor : public AActor
{
	GENERATED_BODY()

public:

	AOvrlItemPickupActor();

public:
	
	virtual void OnConstruction(const FTransform& Transform) override;

protected:

	virtual void BeginPlay() override;

	void ShowItemMesh();

public:

	FORCEINLINE void SetCachedItemInstance(UOvrlItemInstance* ItemToCache) { CachedItemInstance = ItemToCache; }

protected:

	UFUNCTION()
	void OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USphereComponent> PickupCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UStaticMeshComponent> ItemMesh;

	// ------ ITEM ------
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Spawner")
		TObjectPtr<UOvrlPickupDefinition> ItemPickupDefinition;

protected:

	UPROPERTY()
	TObjectPtr<UOvrlItemInstance> CachedItemInstance;

};
