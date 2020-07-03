// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Lady : ModuleRules
{
	public Lady(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "AIModule", "HeadMountedDisplay", "ApexDestruction"});

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
    }
}
