// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlStanceAnimComponentBase.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlPlayerCharacter;
class UCurveVector;

/**
 *
 */
UCLASS(NotBlueprintable)
class OVERLINK_API UOvrlStanceAnimComponentBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void Initialize(AOvrlPlayerCharacter* InPlayerCharacter);

	virtual void Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation);

	virtual void Toggle(bool bEnable);

public:

	UPROPERTY(EditAnywhere, meta = (Categories = "Ovrl.Gait"))
	FGameplayTag GaitToCheck;

	UPROPERTY(EditAnywhere)
	float RecoverySpeed = 2.f;

	FGameplayTag CurrentGait;

protected:

	bool bShouldUpdateAlpha = false;

	FRotator SpineRotation;

	UPROPERTY()
	TWeakObjectPtr<AOvrlPlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TWeakObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	float Alpha = 0.f;
};

UCLASS(Blueprintable, BlueprintType)
class OVERLINK_API UOvrlMoveAnimComponent : public UOvrlStanceAnimComponentBase
{
	GENERATED_BODY()

public:

	virtual void Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation) override;

	virtual void Toggle(bool bEnable) override;

public:


	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	float WalkSwayFrequency;

	// This curve defines the movement of the equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	TObjectPtr<UCurveVector> WalkSwayTranslationCurve;

	// This curve defines the rotation of the equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	TObjectPtr<UCurveVector> WalkSwayRotationCurve;

	// X -> Left/Right
	// Y -> Forward/Backward
	// Z -> Up/Down
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	FVector WalkSwayTranslationMultiplier;

	// X -> Roll
	// Y -> Pitch
	// Z -> Yaw
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	FVector WalkSwayRotationMultiplier;

	// The speed of the walk sway interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Walk Sway")
	float WalkSwaySpeed;

private:

	float WalkSwayTime = 0.f;
	FVector LastWalkSwayTranslation;

};
