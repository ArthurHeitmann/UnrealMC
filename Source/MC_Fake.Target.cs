using UnrealBuildTool;
using System.Collections.Generic;

public class MC_FakeTarget : TargetRules
{
	public MC_FakeTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "MC_Fake" } );
	}
}
