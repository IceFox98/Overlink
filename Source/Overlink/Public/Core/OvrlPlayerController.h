// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OvrlPlayerController.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlCharacterBase;

/**
 *
 */
UCLASS()
class OVERLINK_API AOvrlPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AOvrlPlayerController();

public:
	virtual void UpdateRotation(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	FORCEINLINE FRotator GetLastRotationInput() const { return LastRotationInput; };

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
		TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
		TObjectPtr<AOvrlCharacterBase> OwningCharacter;

	FVector LastFrameGravity = FVector::ZeroVector;

	FRotator LastRotationInput;
};
