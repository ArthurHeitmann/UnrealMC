using UnrealBuildTool;
using System.Collections.Generic;

public class MC_FakeEditorTarget : TargetRules
{
	public MC_FakeEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "MC_Fake" } );
	}
}
