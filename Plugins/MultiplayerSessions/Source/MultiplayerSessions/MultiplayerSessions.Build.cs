// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultiplayerSessions : ModuleRules
{
    public MultiplayerSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
			}
        );

        PrivateIncludePaths.AddRange(
            new string[] {
				// ... add other private include paths required here ...
			}
        );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "OnlineSubsystemSteam",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "UMG",
                "Slate",
                "SlateCore",
                "DeveloperSettings"
				// ... add other public dependencies that you statically link with here ...
			}
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore"
				// "DeveloperToolSettings" foi removido daqui
				// ... add private dependencies that you statically link with here ...	
			}
        );

        // Isolamento do módulo de ferramentas do editor
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("DeveloperToolSettings");
        }

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
				// ... add any modules that your module loads dynamically here ...
			}
        );
    }
}