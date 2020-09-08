// Copyright 2019-2020 James Kelly, Michael Burdge

using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
	public class NumbskullSerialization : ModuleRules
	{
		public NumbskullSerialization(ReadOnlyTargetRules Target) : base(Target)
		{
			PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
				"Core",
				}
				);


			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
				"CoreUObject",
				"Engine",
				}
				);
		}
	}
}