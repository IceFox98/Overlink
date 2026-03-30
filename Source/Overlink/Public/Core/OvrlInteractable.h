// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OvrlInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOvrlInteractable : public UInterface
{
	GENERATED_BODY()
};

class OVERLINK_API IOvrlInteractable
{
	GENERATED_BODY()

public:
	
	// Called when any Interaction Component will hover this object.
	// NOTE: This will not be called if CanInteract() returns false.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	void BeginHover(APawn* InstigatorPawn);

	// Called every game tick, only after BeginHover.
	// NOTE: This will not be called if CanInteract() returns false.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	void HoveringUpdate(const FHitResult& HitResult);

	// Called when any Interaction Component will un-hover this object.
	// NOTE: This will not be called if CanInteract() returns false.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	void EndHover();

	// Is the interaction allowed by this object?
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	bool CanInteract();
	
	// Starts the interaction logic of the object.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	void Interact();

	// Ends the interaction logic of the object.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	void EndInteract();
	
	// If returns true, the Interaction Component will call HoveringUpdate() on this object every tick
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Ovrl Interactable")
	bool ShouldReceiveHoveringUpdate();
};
