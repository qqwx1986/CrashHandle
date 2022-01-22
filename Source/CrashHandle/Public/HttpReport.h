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
	static class UCrashHandleSetting* Setting;
	static void Reporter(FPlatformType Type,const FString& LogFile,const FString& DumpInfo);
	static void RegisterCrashHandler();
};
