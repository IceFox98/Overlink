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
	AddConsoleCommand(AutoCompleteCommands, "ShowDebug Ovrl.Traversals", "Displays traversals traces.");
	AddConsoleCommand(AutoCompleteCommands, "ShowDebug Ovrl.Wallrun", "Displays wallrun traces.");
	AddConsoleCommand(AutoCompleteCommands, "ShowDebug Ovrl.Weapons", "Displays weapon traces.");
}

void FOverlinkModule::AddConsoleCommand(TArray<FAutoCompleteCommand>& AutoCompleteCommands, const FString& Command, const FString& Description)
{
	const auto CommandColor{ GetDefault<UConsoleSettings>()->AutoCompleteCommandColor };

	FAutoCompleteCommand* NewCommand = &AutoCompleteCommands.AddDefaulted_GetRef();
	NewCommand->Command = Command;
	NewCommand->Desc = Description;
	NewCommand->Color = CommandColor;
}
#endif
