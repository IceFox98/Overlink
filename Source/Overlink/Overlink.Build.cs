// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Overlink : ModuleRules
{
	public Overlink(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
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
			"PhysicsCore",
			"CommonUI",
			"Niagara",
			"GameplayMessageRuntime",
			"AnimationWarpingRuntime",
			"AnimGraphRuntime"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"MotionTrajectory",
			"PoseSearch"
		});

		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.Add("AnimGraph");
		}

		OptimizeCode = CodeOptimization.InShippingBuildsOnly;
	}
}