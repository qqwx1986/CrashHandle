// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrashHandle : ModuleRules
{
	protected virtual bool bPlatformSupportsLibCurl
	{
		get
		{
			return Target.Platform.IsInGroup(UnrealPlatformGroup.Windows) ||
			       Target.IsInPlatformGroup(UnrealPlatformGroup.Unix) ||
			       Target.IsInPlatformGroup(UnrealPlatformGroup.Android);
				//||Target.Platform == UnrealTargetPlatform.Switch;
		}
	}
	public CrashHandle(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Http",
				"Json",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
		
		if (bPlatformSupportsLibCurl)
		{
			//AddEngineThirdPartyPrivateStaticDependencies(Target, "libcurl");
			PrivateDependencyModuleNames.AddRange(
				new string[] {
					"Sockets",
				}
			);
			PublicDefinitions.Add("WITH_LIBCURL=1");
		}
		else
		{
			PublicDefinitions.Add("WITH_LIBCURL=0");
		}
	}
}
