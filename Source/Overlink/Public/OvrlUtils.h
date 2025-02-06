// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OBM_Utils.generated.h"

#define LOG_ENABLED 1

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
#define OBM_LOG_INTERNAL(LogCat, PrintScreen, Verbosity, Color, FormatString, ...) \
 \
	UE_LOG(LogCat, Verbosity, TEXT("%s: %s"), *JOYSTR_CUR_FUNC_CLASS_LINE, *FString::Printf(TEXT(FormatString), ##__VA_ARGS__)); \
	if(PrintScreen) \
		{ \
			GEngine->AddOnScreenDebugMessage(-1, 5.f, Color, *(JOYSTR_CUR_FUNC_CLASS_LINE + ": " + FString::Printf(TEXT(FormatString), ##__VA_ARGS__)));\
	    }
#else
#define OBM_LOG_INTERNAL(LogCat, PrintScreen, Verbosity, Color, FormatString, ...)
#endif

// Fast logging
#define OBM_LOG(FormatString, ...) OBM_LOG_INTERNAL(LogTemp, true, Warning, FColor::FromHex("00D4FFFF"), FormatString, ##__VA_ARGS__)

// Error log
#define OBM_LOG_ERR(LogCat, PrintScreen, FormatString, ...) OBM_LOG_INTERNAL(LogCat, PrintScreen, Error, FColor::Red, FormatString, ##__VA_ARGS__)

// Info log
#define OBM_LOG_INFO(LogCat, PrintScreen, FormatString, ...) OBM_LOG_INTERNAL(LogCat, PrintScreen, Display, FColor::FromHex("00D4FFFF"), FormatString, ##__VA_ARGS__)

// Warning log
#define OBM_LOG_WARN(LogCat, PrintScreen, FormatString, ...) OBM_LOG_INTERNAL(LogCat, PrintScreen, Warning, FColor::Orange, FormatString, ##__VA_ARGS__)

/**
 *
 */
UCLASS()
class OVERLINK_API UOBM_Utils : public UBlueprintFunctionLibrary
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
	UFUNCTION(BlueprintPure)
		static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
		static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

};
