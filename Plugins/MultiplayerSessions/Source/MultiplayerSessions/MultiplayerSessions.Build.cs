// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

public class MultiplayerSessions : ModuleRules
{
    public MultiplayerSessions(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UMG",
                "Slate",
                "SlateCore",
                "DeveloperSettings",

                // OSSv2 (Online Services) - Módulos válidos
                "CoreOnline",
                "OnlineServicesInterface",
                "OnlineServicesCommon",
                "OnlineServicesNull",

                // Legacy OSS (Mantenha se estiver usando a API antiga do OnlineSubsystem)
                "OnlineSubsystem",
                "OnlineSubsystemNull",
                "OnlineSubsystemUtils"
            }
        );

        PrivateDependencyModuleNames.AddRange(new string[] { });
        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}