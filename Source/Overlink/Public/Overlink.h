// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

OVERLINK_API DECLARE_LOG_CATEGORY_EXTERN(LogOverlink, Log, All);

struct FAutoCompleteCommand;

class OVERLINK_API FOverlinkModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:

#if ALLOW_CONSOLE
	void OnRegisterConsoleAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);

	void AddConsoleCommand(TArray<FAutoCompleteCommand>& AutoCompleteCommands, const FString& Command, const FString& Description);
#endif
};
