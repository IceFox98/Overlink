// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

struct FAutoCompleteCommand;

class OVERLINK_API FOverlinkModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

private:

#if ALLOW_CONSOLE
	void OnRegisterConsoleAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands);
#endif
};
