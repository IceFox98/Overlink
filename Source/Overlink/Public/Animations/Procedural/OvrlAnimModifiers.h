// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlAnimModifiers.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlPlayerCharacter;
class UCurveVector;
class UOvrlAnimAlphaModifierBase;

USTRUCT()
struct FModifierData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float Frequency = 1.f;

	// Translation curve that the modifier will apply (component space)
	// X -> Left/Right
	// Y -> Forward/Backward
	// Z -> Up/Down 
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> TranslationCurve;
	
	// Translation curve that the modifier will apply (component space)
	// X -> Pitch
	// Y -> Roll
	// Z -> Yaw
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> RotationCurve;

	// X -> Left/Right
	// Y -> Forward/Backward
	// Z -> Up/Down
	UPROPERTY(EditAnywhere)
	FVector TranslationMultiplier;

	// X -> Pitch
	// Y -> Roll
	// Z -> Yaw
	UPROPERTY(EditAnywhere)
	FVector RotationMultiplier;

	float Time = 0.f;
};

/**
 * Simple common behaviors shared between modifiers.
 * Should NOT be used as instanced modifier.
 */
UCLASS(NotBlueprintable, Abstract)
class OVERLINK_API UOvrlAnimModifierBase : public UObject
{
	GENERATED_BODY()

public:
	
	void Initialize(AOvrlPlayerCharacter* InPlayerCharacter);
	void Toggle(bool bEnable);

	void Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation);

	void SetTag(const FGameplayTag& NewTag);

protected:

	virtual void UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation) {};

	void ComputeAlpha(float DeltaTime);
	float GetAlpha();

protected:

	// Used just for debugging purposes
	UPROPERTY(EditAnywhere, Category = "Ovrl Anim Modifier Base")
	bool bEnabled = true;

	// Represents the list of transformation that the modifier will apply
	UPROPERTY(EditAnywhere, Category = "Ovrl Anim Modifier Base")
	TArray<FModifierData> DataList;

	// Gameplay Tag that the player must have to enable this modifier
	UPROPERTY(EditAnywhere, Category = "Ovrl Anim Modifier Base", meta = (Categories = "Ovrl.Gait,Ovrl.LocomotionAction"))
	FGameplayTag TagToCheck;

	UPROPERTY(EditAnywhere, Category = "Ovrl Anim Modifier Base")
	float RecoverySpeed = 1.f;

	UPROPERTY(EditAnywhere, Instanced, Category = "Ovrl Anim Modifier Base")
	TArray<TObjectPtr<UOvrlAnimAlphaModifierBase>> AlphaModifiers;

protected:

	// Don't use this directly, use GetAlpha() instead.
	float Alpha = 0.f;
	bool bShouldUpdateAlpha = false;

	FGameplayTag CurrentTag;

	UPROPERTY()
	TWeakObjectPtr<AOvrlPlayerCharacter> PlayerCharacter;

	UPROPERTY()
	TWeakObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;
};

/**
 * Anim modifiers that is applied during a locomotion action, like walking or running
 */
UCLASS(Blueprintable, Abstract)
class OVERLINK_API UOvrlLocomotionActionsAnimModifier : public UOvrlAnimModifierBase
{
	GENERATED_BODY()

public:

	virtual void UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation) override;

public:

	// The speed of the walk sway interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Locomotion Actions Anim Modifier")
	float InterpSpeed;

protected:

	FVector LastTranslation;
	FRotator LastRotation;
};

UCLASS(Blueprintable, Abstract)
class OVERLINK_API UOvrlMovementAnimModifier : public UOvrlLocomotionActionsAnimModifier
{
	GENERATED_BODY()

public:

	virtual void UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation) override;
};