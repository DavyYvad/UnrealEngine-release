// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright 2018 Nicholas Frechette. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class ACLPlugin : ModuleRules
	{
		public ACLPlugin(ReadOnlyTargetRules Target) : base(Target)
		{
			CppStandard = CppStandardVersion.Cpp17;

            string ACLSDKDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "../ThirdParty"));

			// Replace with PCHUsageMode.UseExplicitOrSharedPCHs when this plugin can compile with cpp20
			PCHUsage = PCHUsageMode.NoPCHs;

			PublicSystemIncludePaths.Add(Path.Combine(ACLSDKDir, "acl/includes"));
			PublicSystemIncludePaths.Add(Path.Combine(ACLSDKDir, "acl/external/rtm/includes"));
			PublicSystemIncludePaths.Add(Path.Combine(ACLSDKDir, "acl/external/sjson-cpp/includes"));

			PublicDependencyModuleNames.Add("Core");
			PublicDependencyModuleNames.Add("CoreUObject");
			PublicDependencyModuleNames.Add("Engine");

			if (Target.bBuildEditor)
			{
				PrivateDependencyModuleNames.Add("DesktopPlatform");
				PrivateDependencyModuleNames.Add("UnrealEd");
			}

			if (Target.Platform == UnrealTargetPlatform.Linux)
			{
				// There appears to be a bug when cross-compiling Linux under Windows where the clang tool-chain used
				// isn't fully C++11 compliant. The standard specifies that when the 'cinttypes' header is included
				// the format macros are always defined unlike C which requires the following macro to be defined first.
				// This fix should be required for UE 4.20 and earlier versions.
				PrivateDefinitions.Add("__STDC_FORMAT_MACROS");
			}
		}
	}
}
