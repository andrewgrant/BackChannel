// Copyright 2017 Andrew Grant
// Unless explicitly stated otherwise all files in this repository 
// are licensed under BSD License 2.0. All Rights Reserved.

using UnrealBuildTool;

public class BackChannel : ModuleRules
{
	public BackChannel( ReadOnlyTargetRules Target ) : base( Target )
	{
		PublicIncludePaths.AddRange(
			new string[] {
				ModuleDirectory
			}
		);

		PrivateDependencyModuleNames.AddRange
			(
			new string[] {
				"Core",
				"CoreUObject",
				"Engine",
				"Sockets",
				"Networking"
			}
		);
	}
}