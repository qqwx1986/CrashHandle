// Fill out your copyright notice in the Description page of Project Settings.


#include "CrashHandleBlueprintLibrary.h"

#include "CrashHandleSetting.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "CrashHandle.h"

FString UCrashHandleBlueprintLibrary::BinaryVersion;
FString UCrashHandleBlueprintLibrary::ResourceVersion = TEXT("1.0");

TSharedPtr<FJsonObject> UCrashHandleBlueprintLibrary::GetDeviceInfo()
{
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	FString Platform, DeviceId;
#if PLATFORM_WINDOWS
	Platform = "windows";
	DeviceId = FPlatformMisc::GetLoginId();
#endif
#if PLATFORM_ANDROID
	Platform = "android";
	DeviceId = FPlatformMisc::GetDeviceId();
#endif
#if PLATFORM_LINUX
	Platform = "linux";
	DeviceId = FPlatformMisc::GetLoginId();
#endif
#if PLATFORM_IOS
	Platform = "ios";
	DeviceId = FPlatformMisc::GetDeviceId();
#endif
#if PLATFORM_MAC
	Platform = "mac";
	DeviceId = FPlatformMisc::GetLoginId();
#endif
	JsonObject->SetStringField(TEXT("Platform"), Platform);
	JsonObject->SetStringField(TEXT("DeviceId"), DeviceId);
	JsonObject->SetStringField(TEXT("Cpu"), FPlatformMisc::GetCPUBrand());
	JsonObject->SetStringField(TEXT("Gpu"), FPlatformMisc::GetPrimaryGPUBrand());
	JsonObject->SetNumberField(TEXT("Memory"), FPlatformMemory::GetConstants().TotalPhysicalGB);
	FString OSVersionLabel, OSSubVersionLabel;
	FPlatformMisc::GetOSVersions(OSVersionLabel, OSSubVersionLabel);
	JsonObject->SetStringField(TEXT("Os"), OSVersionLabel);
	JsonObject->SetStringField(TEXT("OsSub"), OSSubVersionLabel);
	JsonObject->SetStringField(TEXT("OsVersion"), FPlatformMisc::GetOSVersion());
	return JsonObject;
}

void UCrashHandleBlueprintLibrary::ReportError(const FString& Error)
{
	const UCrashHandleSetting& Settings = *GetDefault<UCrashHandleSetting>();
#if !UE_BUILD_SHIPPING
	if (!Settings.bEnableDev)
	{
		return;
	}
#endif
	if (Settings.ServerUrl.IsEmpty() || Settings.ErrorReportPath.IsEmpty())
	{
		return;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	JsonObject->SetStringField(TEXT("Error"), Error);
	JsonObject->SetStringField(TEXT("ErrorGuid"), FGuid::NewGuid().ToString());
	JsonObject->SetStringField(TEXT("Version"), ResourceVersion);
	JsonObject->SetStringField(TEXT("Time"), FDateTime::Now().ToIso8601());
	JsonObject->SetObjectField(TEXT("DeviceInfo"), GetDeviceInfo());
	FString ErrorJson;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ErrorJson);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FString URL = Settings.ServerUrl + Settings.ErrorReportPath;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[=](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
		{
			if (bSucceeded)
			{
				UE_LOG(CrashHandle, Log, TEXT("Http Rsp %s"), *HttpResponse->GetContentAsString());
			}
			else
			{
				UE_LOG(CrashHandle, Error, TEXT("Http Rsp Error %d"), HttpResponse->GetResponseCode());
			}
		});
	HttpRequest->SetURL(URL);
	HttpRequest->SetContentAsString(ErrorJson);
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->ProcessRequest();
}


void UCrashHandleBlueprintLibrary::UpdateResourceVersion(const FString& Version)
{
	ResourceVersion = Version;
}

bool UCrashHandleBlueprintLibrary::EnableErrorReport()
{
	const UCrashHandleSetting& Settings = *GetDefault<UCrashHandleSetting>();
#if !UE_BUILD_SHIPPING
	if (!Settings.bEnableDev)
	{
		return false;
	}
#endif
	return (!Settings.ServerUrl.IsEmpty()) && (!Settings.ErrorReportPath.IsEmpty());
}

void UCrashHandleBlueprintLibrary::CrashTest()
{
	int* a = nullptr;
	*a = 1;
}

void UCrashHandleBlueprintLibrary::InitVersion()
{
	const UCrashHandleSetting& Settings = *GetDefault<UCrashHandleSetting>();
#if !UE_BUILD_SHIPPING
	if (!Settings.bEnableDev)
	{
		return;
	}
#endif
#if PLATFORM_WINDOWS || PLATFORM_LINUX || PLATFORM_MAC
	if (Settings.bGenCrashConfig)
	{
		const FString EngineBinariesDirectory = FPaths::Combine(FPlatformMisc::EngineDir(), TEXT("Binaries"),
		                                                        FPlatformProcess::GetBinariesSubdirectory());
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*EngineBinariesDirectory))
		{
			PlatformFile.CreateDirectory(*EngineBinariesDirectory);
		}
		FString ExecutePath = FPlatformProcess::ExecutablePath();
		FMD5Hash Hash = FMD5Hash::HashFile(*ExecutePath);
		FString Md5Str = LexToString(Hash);

		TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
		JsonObject->SetStringField(TEXT("Url"), Settings.ServerUrl);
		JsonObject->SetStringField(TEXT("Ver"), Md5Str);

		FString StrJson;
		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&StrJson);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
		FString ConfigFile = EngineBinariesDirectory / TEXT("CrashReportClient.json");
		if (IFileManager::Get().FileExists(*ConfigFile))
		{
			IFileManager::Get().Delete(*ConfigFile);
		}
		FArchive* FileWriter = IFileManager::Get().CreateFileWriter(*ConfigFile);
		if (FileWriter)
		{
			FileWriter->Serialize(TCHAR_TO_ANSI(*StrJson), StrJson.Len());
			FileWriter->Close();
			delete FileWriter;
		}
	}
#endif
}
