// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "Interactives/Interactable.h"
#include "InteractionComponent.generated.h"

USTRUCT(BlueprintType)
struct FInteractionData
{
public:

	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere)
		AActor* HitActor;

	UPROPERTY(VisibleAnywhere)
		UPrimitiveComponent* HitComponent;

	UPROPERTY(VisibleAnywhere)
		bool IsInteractive;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ONEBULLETMAN_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FInteractionData FindInteractiveObject();

	UFUNCTION(BlueprintCallable)
		void ChangeRange(float NewRange);

protected:

	UPROPERTY()
		class ACharacterBase* CharacterRef;

	UPROPERTY(EditAnywhere, Category = "Item Picker", meta = (ClampMin = 0.1f))
		float Range;

private:

	UPROPERTY()
		AActor* LastActorHit;

	FVector StartPoint;
};
