// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrashHandle.h"

#include "CrashHandleBlueprintLibrary.h"
#include "CrashHandleSetting.h"
#include "HttpReport.h"
#define LOCTEXT_NAMESPACE "FCrashHandleModule"
DEFINE_LOG_CATEGORY(CrashHandle);

void FCrashHandleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	const TCHAR* SectionName = TEXT("/Script/CrashHandle.CrashHandleSetting");
	const FString ConfigIniPath = FPaths::SourceConfigDir().Append(TEXT("DefaultCrashHandle.ini"));
	UCrashHandleSetting& Settings = *GetMutableDefault<UCrashHandleSetting>();
	if (GConfig->DoesSectionExist(SectionName, ConfigIniPath))
	{
		GConfig->GetBool(SectionName, TEXT("bEnableDev"), Settings.bEnableDev, ConfigIniPath);
		GConfig->GetBool(SectionName, TEXT("bGenCrashConfig"), Settings.bGenCrashConfig, ConfigIniPath);
		GConfig->GetBool(SectionName, TEXT("bAndroidCrashReport"), Settings.bAndroidCrashReport, ConfigIniPath);
		GConfig->GetBool(SectionName, TEXT("bIOSCrashReport"), Settings.bIOSCrashReport, ConfigIniPath);
		GConfig->GetBool(SectionName, TEXT("bLinuxCrashReport"), Settings.bLinuxCrashReport, ConfigIniPath);
		GConfig->GetString(SectionName,TEXT("ServerUrl"), Settings.ServerUrl, ConfigIniPath);
		GConfig->GetString(SectionName,TEXT("DevServerUrl"), Settings.DevServerUrl, ConfigIniPath);
		GConfig->GetString(SectionName,TEXT("ErrorReportPath"), Settings.ErrorReportPath, ConfigIniPath);
		GConfig->GetString(SectionName,TEXT("CrashReportPath"), Settings.CrashReportPath, ConfigIniPath);
		GConfig->GetString(SectionName,TEXT("Version"), Settings.Version, ConfigIniPath);
	}
#if UE_EDITOR
	Settings.bEnableDev = false;
#endif
	if (Settings.bEnableDev)
	{
		Settings.ServerUrl = Settings.DevServerUrl;
	}
	FHttpReport::RegisterCrashHandler();
	UCrashHandleBlueprintLibrary::InitVersion(Settings.Version);
}

void FCrashHandleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCrashHandleModule, CrashHandle)
