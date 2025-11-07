// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

public class DarkAnthology : ModuleRules
{
	public DarkAnthology(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"RenderCore",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});
		
		PrivateIncludePaths.AddRange(
			new string[]
			{
				"DarkAnthology/Game"
			});
	}
}