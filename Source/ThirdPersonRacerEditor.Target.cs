using UnrealBuildTool;
using System.Collections.Generic;

public class ThirdPersonRacerEditorTarget : TargetRules
{
    public ThirdPersonRacerEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("ThirdPersonRacer");
    }
}
