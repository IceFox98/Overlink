// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

OVERLINKEDITOR_API DECLARE_LOG_CATEGORY_EXTERN(LogOverlinkEditor, Log, All);

class OVERLINKEDITOR_API FOverlinkEditorModule : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
