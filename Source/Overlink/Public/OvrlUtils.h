// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OvrlUtils.generated.h"

class APawn;
class ACharacter;
enum class ECameraFeedbackEvent : uint8;
struct FAnimNodeReference;
struct FOvrlItemEntry;
struct FCharacterTrajectoryData;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Draw a debug string at a 3d world location. */
	UFUNCTION(BlueprintCallable, Category="Ovrl Utils", meta=(WorldContext="WorldContextObject", DevelopmentOnly))
	static void OvrlDrawDebugString(const UObject* WorldContextObject, const FVector TextLocation, const FString& Text, class AActor* TestBaseActor = NULL, FLinearColor TextColor = FLinearColor::White, float Duration = 0.f, float FontScale = 1.f);

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils", meta = (BlueprintThreadSafe))
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	UFUNCTION(BlueprintPure, Category = "Ovrl Utils", meta = (BlueprintThreadSafe))
	static FQuat GetGravityRelativeRotation_Quat(FQuat Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

	static FTransform ExtractRootTransformFromMontage(const UAnimMontage* Montage, float Time);

	static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Utils", meta=(AutoCreateRefTerm="WorldObjectContext"))
	static void TriggerCameraEvent(UObject* WorldObjectContext, ECameraFeedbackEvent CameraEvent);

	// Looks for an Inventory Component and retrieve the first item matching the passed definition.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static UPARAM(DisplayName="Item") FOvrlItemEntry GetFirstItemEntry(AActor* InventoryOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition);

	// Looks for an Equipment Manager Component and retrieve the first equipment instance matching the passed definition.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static UPARAM(DisplayName="Equipment Instance") AOvrlEquipmentInstance* GetFirstEquipmentInstance(AActor* InventoryOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition);
	
	UFUNCTION(BlueprintCallable, Category = "Ovrl Utils", meta = (BlueprintThreadSafe, DisplayName = "Ovrl Generate Character Trajectory"))
	static void GenerateCharacterTrajectory(const ACharacter* InCharacter, 
		FCharacterTrajectoryData InTrajectoryData, float InDeltaTime,
		UPARAM(ref) FPoseSearchQueryTrajectory& InOutTrajectory,
		UPARAM(ref) TArray<FVector>& OutTranslationHistory, FPoseSearchQueryTrajectory& OutTrajectory,
		float InHistorySamplingInterval = 0.04f, int32 InTrajectoryHistoryCount = 10, float InPredictionSamplingInterval = 0.2f, int32 InTrajectoryPredictionCount = 8);
};
