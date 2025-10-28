using UnrealBuildTool;
using System.Collections.Generic;

public class RacerTarget : TargetRules
{
    public RacerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
        ExtraModuleNames.AddRange(new string[] { "Racer" });
    }
}
