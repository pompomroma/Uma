using UnrealBuildTool;

public class Racer : ModuleRules
{
    public Racer(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "Slate",
            "SlateCore",
            "UMG",
            "Niagara"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "AIModule",
            "NavigationSystem",
            "GameplayTasks"
        });
    }
}
