using UnrealBuildTool;
using System.Collections.Generic;

public class RacerEditorTarget : TargetRules
{
    public RacerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_2;
        ExtraModuleNames.AddRange(new string[] { "Racer" });
    }
}
