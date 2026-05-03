// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverlinkEditor.h"

#include "EditorUtilitySubsystem.h"
#include "EditorUtilityWidgetBlueprint.h"

class UEditorUtilitySubsystem;

IMPLEMENT_GAME_MODULE(FOverlinkEditorModule, OverlinkEditor);

DEFINE_LOG_CATEGORY(LogOverlinkEditor);

void FOverlinkEditorModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
	
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FOverlinkEditorModule::RegisterMenus)
	);
}

void FOverlinkEditorModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

void FOverlinkEditorModule::RegisterMenus()
{
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	
	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");

	FToolMenuEntry Entry = FToolMenuEntry::InitToolBarButton(
		"CreateItemButton",
		FUIAction(FExecuteAction::CreateRaw(this, &FOverlinkEditorModule::OnButtonClicked)),
		FText::FromString("Create Item"),
		FText::FromString("Open Item Generator Utility"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "AutomationTools.MenuIcon")
	);

	Entry.StyleNameOverride = "CalloutToolbar";
	
	Section.AddEntry(Entry);
}

void FOverlinkEditorModule::OnButtonClicked()
{
	FString Path = "/Game/Core/EditorUtils/ItemGeneratorUtility.ItemGeneratorUtility";

	UObject* Asset = LoadObject<UObject>(nullptr, *Path);

	if (UEditorUtilityWidgetBlueprint* WidgetBP = Cast<UEditorUtilityWidgetBlueprint>(Asset))
	{
		UEditorUtilitySubsystem* Subsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>();
		Subsystem->SpawnAndRegisterTab(WidgetBP);
	}
}
