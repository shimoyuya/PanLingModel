// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PanLing : ModuleRules
{
	public PanLing(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
        PublicDependencyModuleNames.AddRange(new string[] { "AIModule", "GameplayTasks", "NavigationSystem", "UMG" });
    }
}
