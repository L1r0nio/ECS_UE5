// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

public class DarkAnthology : ModuleRules
{
	public DarkAnthology(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"EnhancedInput",
			"Core",
			"RenderCore",
			"CoreUObject",
			"Engine",
			"InputCore"
		});
		
		PrivateIncludePaths.AddRange(
			new string[]
			{
				"DarkAnthology/Game"
			});
	}
}