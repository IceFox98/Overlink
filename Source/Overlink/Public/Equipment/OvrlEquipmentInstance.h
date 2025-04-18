// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "OvrlEquipmentInstance.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlInventoryComponent;
class UOvrlItemInstance;
class UCurveVector;

UCLASS()
class OVERLINK_API AOvrlEquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AOvrlEquipmentInstance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual void OnEquipped();
	virtual void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Equipped"))
		void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Unequipped"))
		void K2_OnUnequipped();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
		FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };
	FORCEINLINE UCurveVector* GetWalkSwayCurve() const { return WalkSwayCurve; };

protected:

	void ApplyOverlayAnimation();

public:

	// This curve defines the movement of this equipped item while the player is walking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Instance|Sway")
		TObjectPtr<UCurveVector> WalkSwayCurve;

	// The maximum sway rotation limit the equipped item can reach on both axis X and Y
	// X -> Yaw Sway
	// Y -> Pitch Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Instance|Sway")
		FVector2D SwayRotationLimit;

	// The maximum sway movement limit the equipped can reach on all axis
	// X -> Forward/Backward Sway
	// Y -> Right/Left Sway
	// Z -> Up/Down Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Instance|Sway")
		FVector SwayMovementLimit;

protected:

	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Instance")
		TSubclassOf<UOvrlEquipmentDefinition> EquipmentDefinitionClass;

	UPROPERTY()
		TObjectPtr<UOvrlItemInstance> AssociatedItem;

private:

	friend class UOvrlInventoryComponent;

	// List of granted handles
	FOvrlAbilitySet_GrantedHandles GrantedHandles;

	bool bIsEquipped = false;

	UPROPERTY()
		TObjectPtr<USceneComponent> TargetToFollow;

	FVector RelativeLocation;
};
