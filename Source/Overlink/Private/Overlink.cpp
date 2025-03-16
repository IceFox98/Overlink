// Copyright Epic Games, Inc. All Rights Reserved.

#include "Overlink.h"

#if ALLOW_CONSOLE
#include "Engine/Console.h"
#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FOverlinkModule, Overlink, "Overlink");

DEFINE_LOG_CATEGORY(LogOverlink);

void FOverlinkModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();

#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.AddRaw(this, &FOverlinkModule::OnRegisterConsoleAutoCompleteEntries);
#endif
}

void FOverlinkModule::ShutdownModule()
{
#if ALLOW_CONSOLE
	UConsole::RegisterConsoleAutoCompleteEntries.RemoveAll(this);
#endif

	FDefaultGameModuleImpl::ShutdownModule();
}

#if ALLOW_CONSOLE
void FOverlinkModule::OnRegisterConsoleAutoCompleteEntries(TArray<FAutoCompleteCommand>& AutoCompleteCommands)
{
	const auto CommandColor{ GetDefault<UConsoleSettings>()->AutoCompleteCommandColor };

	FAutoCompleteCommand* Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command = &AutoCompleteCommands.AddDefaulted_GetRef();
	Command->Command = FString{ TEXTVIEW("ShowDebug Ovrl.Traversals") };
	Command->Desc = FString{ TEXTVIEW("Displays traversals traces.") };
	Command->Color = CommandColor;
}
#endif
