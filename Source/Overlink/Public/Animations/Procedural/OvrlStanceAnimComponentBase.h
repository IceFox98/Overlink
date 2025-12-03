// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "Engine/DataAsset.h"
#include "OvrlStanceAnimComponentBase.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlPlayerCharacter;
class UCurveVector;

UCLASS()
class USwayCurveData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float Frequency;

	// This curve defines the movement of the equipped item while the player is walking
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> TranslationCurve;

	// This curve defines the rotation of the equipped item while the player is walking
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> RotationCurve;

	// X -> Left/Right
	// Y -> Forward/Backward
	// Z -> Up/Down
	UPROPERTY(EditAnywhere)
	FVector TranslationMultiplier;

	// X -> Roll
	// Y -> Pitch
	// Z -> Yaw
	UPROPERTY(EditAnywhere)
	FVector RotationMultiplier;

	float Time = 0.f;
};


/**
 *
 */
UCLASS(NotBlueprintable, Abstract)
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

public:

	UPROPERTY(EditAnywhere, Category = "Ovrl Move Anim Component")
	TArray<TObjectPtr<USwayCurveData>> CurvesData;

	//UPROPERTY(EditAnywhere, Category = "Ovrl Move Anim Component")
	//TObjectPtr<USwayCurveData> LoopCurveData;

	// The speed of the walk sway interpolation
	UPROPERTY(EditAnywhere)
	float SwaySpeed;

private:

	float WalkSwayTime = 0.f;
	FVector LastWalkSwayTranslation;

	float TargetResetTime = 0.f;

};
