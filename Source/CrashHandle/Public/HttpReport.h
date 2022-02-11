#pragma once

class CRASHHANDLE_API FHttpReport
{
public:
	enum FPlatformType
	{
		Android,
		IOS,
		Linux,
	};
	static void Reporter(FPlatformType Type,const FString& LogFile,const FString& DumpInfo);
	static void RegisterCrashHandler();
};
