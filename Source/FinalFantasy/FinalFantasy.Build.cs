// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FinalFantasy : ModuleRules
{
	public FinalFantasy(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
            "OnlineSubsystemSteam",
            "OnlineSubsystem",
            "OnlineSubsystemUtils"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"FinalFantasy",
			"FinalFantasy/Variant_Platforming",
			"FinalFantasy/Variant_Platforming/Animation",
			"FinalFantasy/Variant_Combat",
			"FinalFantasy/Variant_Combat/AI",
			"FinalFantasy/Variant_Combat/Animation",
			"FinalFantasy/Variant_Combat/Gameplay",
			"FinalFantasy/Variant_Combat/Interfaces",
			"FinalFantasy/Variant_Combat/UI",
			"FinalFantasy/Variant_SideScrolling",
			"FinalFantasy/Variant_SideScrolling/AI",
			"FinalFantasy/Variant_SideScrolling/Gameplay",
			"FinalFantasy/Variant_SideScrolling/Interfaces",
			"FinalFantasy/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
