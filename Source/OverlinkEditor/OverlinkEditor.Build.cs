// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OverlinkEditor : ModuleRules
{
	public OverlinkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Overlink",
				"AnimGraph",
				"AnimGraphRuntime",
				"SlateCore",
			});

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"BlueprintGraph",
					"EditorFramework",
					"Kismet",
					"UnrealEd",
					"Blutility",
					"UMG",
					"UMGEditor",
					"BlueprintEditorLibrary",
					"ToolWidgets",
					"GameplayAbilities",
				}
			);
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}