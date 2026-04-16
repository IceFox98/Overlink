// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OvrlInteractionComponent.generated.h"

class IOvrlInteractable;

USTRUCT(BlueprintType)
struct FInteractableObjectData
{
	GENERATED_USTRUCT_BODY()

	// Either Component or Actor hit during interaction query
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> OriginalObject = nullptr;

	// Just the interface version of OriginalObject
	UPROPERTY(BlueprintReadOnly)
	TScriptInterface<IOvrlInteractable> InteractableObject = nullptr;

	// Actor that owns the hit component.
	// Can be equals to OriginalObject if that's an Actor.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> OwningActor = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectBeginHovered, UObject*, HoveredObject);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnObjectEndHovered, UObject*, PrevHoveredObject);

UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOvrlInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOvrlInteractionComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Interaction Component")
	FORCEINLINE void SetInteractionDistance(float InNewDistance) { InteractionDistance = InNewDistance; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Interaction Component")
	FORCEINLINE void SetTraceTypes(const TArray<TEnumAsByte<EObjectTypeQuery>>& InTraceTypes) { TraceTypes = InTraceTypes; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Interaction Component")
	FORCEINLINE void SetTraceComplex(bool bShouldTraceComplex) { bTraceComplex = bShouldTraceComplex; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Interaction Component")
	const FInteractableObjectData& GetCurrentPointedObjectData() const { return CurrentPointedObjData; };

protected:
	virtual void BeginPlay() override;

	// Main function that performs the interaction trace, called every tick.
	// Can be overridden for custom logic.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Interaction Component")
	FInteractableObjectData FindInteractableObject() const;
	virtual FInteractableObjectData FindInteractableObject_Implementation() const;

	// Returns the locations of where the interaction trace should start/end.
	// By default, it will use PlayerCameraManager position as start.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Interaction Component")
	void GetTraceStartEnd(FVector& OutTraceStart, FVector& OutTraceEnd) const;
	virtual void GetTraceStartEnd_Implementation(FVector& OutTraceStart, FVector& OutTraceEnd) const;

	// Return true if the passed object is valid and implements UInteractable interface.
	virtual bool IsInteractableObject(UObject* ObjectToCheck) const;

	// Utility function to create the data struct of the interacted object
	FInteractableObjectData CreateInteractableData(UObject* OriginalObject, AActor* OwningActor) const;
	
public:
	
	UPROPERTY(BlueprintAssignable, Category = "Ovrl Interaction Component")
	FOnObjectBeginHovered OnObjectBeginHovered;
	
	UPROPERTY(BlueprintAssignable, Category = "Ovrl Interaction Component")
	FOnObjectEndHovered OnObjectEndHovered;

protected:
	// Distance from interaction trace start position to where it should end.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Interaction Component", meta = (ClampMin = 0.1f, Units = "cm"))
	float InteractionDistance;

	// If true, interaction trace query will search for complex collisions.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Interaction Component")
	bool bTraceComplex;

	// Trace types used during interaction trace query.
	UPROPERTY(EditAnywhere, Category = "Ovrl Interaction Component")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceTypes;

	FInteractableObjectData CurrentPointedObjData;

private:
	UPROPERTY()
	TObjectPtr<APawn> OwningPawn;
};
