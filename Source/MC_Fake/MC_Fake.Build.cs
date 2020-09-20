using UnrealBuildTool;

public class MC_Fake : ModuleRules
{
	public MC_Fake(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseRTTI = true;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { 
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EngineSettings",
            "UMG"
        });

        PublicDependencyModuleNames.Add("RuntimeMeshComponent");
        
        // PublicDependencyModuleNames.AddRange(new string[] { "RenderCore", "RHI", "RuntimeMeshComponent" }); TODO remove

        //PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        //#ifndef UE_4_22_OR_LATER
        //        PublicDependencyModuleNames.AddRange(new string[] { "ShaderCore" });
        //#endif
        /*ublicDependencyModuleNames.AddRange(
            new string[]
            {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "RenderCore",
                    "ShaderCore",
                    "RHI"
            }
        );*/

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
