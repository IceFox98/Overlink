// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Overlink : ModuleRules
{
    public Overlink(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
            "MotionWarping",
            "EngineSettings",
            "UMG",
            "Slate",
            "SlateCore",
            "CommonUI",
        });
    }
}
