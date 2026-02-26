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
				}
			);
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}