// Copyright Epic Games, Inc. All Rights Reserved.

#include "OverlinkEditor.h"

IMPLEMENT_MODULE(FOverlinkEditorModule, OverlinkEditor);

DEFINE_LOG_CATEGORY(LogOverlinkEditor);

void FOverlinkEditorModule::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
}

void FOverlinkEditorModule::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}
