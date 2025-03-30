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

	FAutoCompleteCommand* CommandTraversal = &AutoCompleteCommands.AddDefaulted_GetRef();
	CommandTraversal = &AutoCompleteCommands.AddDefaulted_GetRef();
	CommandTraversal->Command = FString{ TEXTVIEW("ShowDebug Ovrl.Traversals") };
	CommandTraversal->Desc = FString{ TEXTVIEW("Displays traversals traces.") };
	CommandTraversal->Color = CommandColor;

	FAutoCompleteCommand* CommandWeapon = &AutoCompleteCommands.AddDefaulted_GetRef();
	CommandWeapon = &AutoCompleteCommands.AddDefaulted_GetRef();
	CommandWeapon->Command = FString{ TEXTVIEW("ShowDebug Ovrl.Weapons") };
	CommandWeapon->Desc = FString{ TEXTVIEW("Displays weapon traces.") };
	CommandWeapon->Color = CommandColor;
}
#endif
