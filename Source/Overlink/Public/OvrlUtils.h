// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OvrlUtils.generated.h"

#define LOG_ENABLED 1

#define UE_LN2		(0.69314718056f)

//Current Class Name + Function Name where this is called
#define JOYSTR_CUR_CLASS_FUNC (FString(__FUNCTION__))

//Current Class where this is called
#define JOYSTR_CUR_CLASS (FString(__FUNCTION__).Left(FString(__FUNCTION__).Find(TEXT(":"))) )

//Current Function Name where this is called
#define JOYSTR_CUR_FUNC (FString(__FUNCTION__).Right(FString(__FUNCTION__).Len() - FString(__FUNCTION__).Find(TEXT("::")) - 2 ))

//Current Line Number in the code where this is called
#define JOYSTR_CUR_LINE  (FString::FromInt(__LINE__))

//Current Class and Line Number where this is called
#define JOYSTR_CUR_CLASS_LINE (JOYSTR_CUR_CLASS + "(" + JOYSTR_CUR_LINE + ")")

//Current Class and Line Number where this is called
#define JOYSTR_CUR_FUNC_CLASS_LINE (JOYSTR_CUR_CLASS_FUNC + "(" + JOYSTR_CUR_LINE + ")")

//Current Function Signature where this is called
#define JOYSTR_CUR_FUNCSIG (FString(__FUNCSIG__))

#if LOG_ENABLED
#define OVRL_LOG_INTERNAL(LogCat, PrintScreen, Verbosity, Color, FormatString, ...) \
 \
	UE_LOG(LogCat, Verbosity, TEXT("%s: %s"), *JOYSTR_CUR_FUNC_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__)); \
	if(PrintScreen) \
		{ \
			GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, *(JOYSTR_CUR_FUNC_CLASS_LINE + ": " + FString::Printf(TEXT(FormatString), ##__VA_ARGS__)));\
	    }
#else
#define OVRL_LOG_INTERNAL(LogCat, PrintScreen, Verbosity, Color, FormatString, ...)
#endif

// Fast logging
#define OVRL_LOG(FormatString, ...) OVRL_LOG_INTERNAL(LogTemp, true, Warning, FColor::FromHex("00D4FFFF"), FormatString, ##__VA_ARGS__)

// Error log
#define OVRL_LOG_ERR(LogCat, PrintScreen, FormatString, ...) OVRL_LOG_INTERNAL(LogCat, PrintScreen, Error, FColor::Red, FormatString, ##__VA_ARGS__)

// Info log
#define OVRL_LOG_INFO(LogCat, PrintScreen, FormatString, ...) OVRL_LOG_INTERNAL(LogCat, PrintScreen, Display, FColor::FromHex("00D4FFFF"), FormatString, ##__VA_ARGS__)

// Warning log
#define OVRL_LOG_WARN(LogCat, PrintScreen, FormatString, ...) OVRL_LOG_INTERNAL(LogCat, PrintScreen, Warning, FColor::Orange, FormatString, ##__VA_ARGS__)

enum class ECameraFeedbackEvent : uint8;
struct FAnimNodeReference;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static bool IsRunningEditor()
	{
#if WITH_EDITOR
		return true;
#else
		return false;
#endif
	}

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static FQuat GetGravityRelativeRotation_Quat(FQuat Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils")
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

	static FTransform ExtractRootTransformFromMontage(const UAnimMontage* Montage, float Time);

	/** Get the current world space transform from the offset root bone animgraph node */
	UFUNCTION(BlueprintPure, Category = "Ovrl Utils", meta = (BlueprintThreadSafe), DisplayName = "Get Ovrl Offset Root Transform")
	static FTransform GetOffsetRootTransform(const FAnimNodeReference& Node);

	static bool ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName);

	static void TriggerCameraEvent(UObject* WorldObjectContext, ECameraFeedbackEvent CameraEvent);

	static void CriticalSpringDamperQuat(FQuat& InOutRotation, FVector& InOutAngularVelocityRadians, const FQuat& TargetRotation, float SmoothingTime, float DeltaTime);
	
	static constexpr float SmoothingTimeToDamping(float SmoothingTime)
	{
		return 4.0f / FMath::Max(SmoothingTime, UE_KINDA_SMALL_NUMBER);
	}
	
	static constexpr float HalfLifeToSmoothingTime(float HalfLife)
	{
		return HalfLife / UE_LN2;
	}
};
