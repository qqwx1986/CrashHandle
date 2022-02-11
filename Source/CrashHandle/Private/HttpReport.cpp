#include "HttpReport.h"

#include "CrashHandleSetting.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "CrashHandle.h"
#include "CrashHandleBlueprintLibrary.h"
#include "SocketSubsystem.h"
#if PLATFORM_IOS
#include "IOSCrashHandle.h"
#endif
#if PLATFORM_ANDROID
#include "AndroidCrashHandle.h"
#endif
#if PLATFORM_LINUX
#include "LinuxCrashHandle.h"
#endif

static void SendHttpSync(const FString& Platform, const FString& InUrl, const FString& Content);

void FHttpReport::Reporter(FPlatformType Type, const FString& LogFile, const FString& DumpInfo)
{
	FString PlatformName = TEXT("windows");
	switch (Type)
	{
	case Android:
		PlatformName = TEXT("android");
		break;
	case IOS:
		PlatformName = TEXT("ios");
		break;
	case Linux:
		PlatformName = TEXT("linux");
		break;
	default:
		break;
	}
	FString LogInfo;
	if (!LogFile.IsEmpty())
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		FString CrashLogFile = FPaths::ProjectLogDir() / TEXT("Crash.log");
		PlatformFile.CopyFile(*CrashLogFile, *LogFile);
		FFileHelper::LoadFileToString(LogInfo, *CrashLogFile);
	}
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	FString Content;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);
	JsonObject->SetStringField(TEXT("Platform"), PlatformName);
	JsonObject->SetStringField(TEXT("Version"), UCrashHandleBlueprintLibrary::BinaryVersion);
	JsonObject->SetStringField(TEXT("CrashGuid"), FGuid::NewGuid().ToString());
	JsonObject->SetStringField(TEXT("Dump"), DumpInfo);
	JsonObject->SetStringField(TEXT("Log"), LogInfo.Right(500));
	JsonObject->SetObjectField(TEXT("DeviceInfo"), UCrashHandleBlueprintLibrary::GetDeviceInfo());
	FJsonSerializer::Serialize(JsonObject, JsonWriter);
	JsonWriter->Close();

	const UCrashHandleSetting& Settings = *GetDefault<UCrashHandleSetting>();
	SendHttpSync(PlatformName, Settings.ServerUrl + Settings.CrashReportPath, Content);
}

static void DefaultCrashHandler(const FGenericCrashContext& Context)
{
	UE_LOG(CrashHandle, Log, TEXT("DefaultCrashHandler"));
#if PLATFORM_IOS
	FIOSCrashHandle::DefaultCrashHandler(Context);
#endif
#if PLATFORM_ANDROID
	FAndroidCrashHandle::DefaultCrashHandler(Context);
#endif
#if PLATFORM_LINUX
	FLinuxCrashHandle::DefaultCrashHandler(Context);
#endif
}

void FHttpReport::RegisterCrashHandler()
{
	UE_LOG(CrashHandle, Log, TEXT("RegisterCrashHandler"));
	const UCrashHandleSetting& Settings = *GetDefault<UCrashHandleSetting>();
	if (!Settings.bAndroidCrashReport)
	{
#if PLATFORM_ANDROID
		return;
#endif
	}
	if (!Settings.bLinuxCrashReport)
	{
#if PLATFORM_LINUX
		return;
#endif
	}
	if (!Settings.bIOSCrashReport)
	{
#if PLATFORM_IOS
		return;
#endif
	}
#if !UE_BUILD_SHIPPING
	if (!Settings.bEnableDev)
	{
		return;
	}
#endif
	FPlatformMisc::SetCrashHandler(DefaultCrashHandler);
}

#if WITH_LIBCURL
#include "curl/curl.h"

static size_t StaticReadFn(void* Ptr, size_t SizeInBlocks, size_t BlockSizeInBytes, void* UserData)
{
	FString* Content = static_cast<FString*>(UserData);

	FMemory::Memcpy(Ptr, TCHAR_TO_ANSI(**Content), Content->Len());

	return Content->Len();
}

static void SendHttpSync(const FString& Platform, const FString& InUrl, const FString& Content)
{
	FString Url = InUrl;
	auto GetIpFromDomain = [&Url](const FString StartPrefix)-> bool
	{
		FString OldUrl = Url;
		if (!Url.StartsWith(StartPrefix))return false;
		Url = Url.Right(Url.Len() - StartPrefix.Len());
		FString Domain;
		FString Port;
		if (!Url.Split(":", &Domain, &Port))
		{
			Domain = Url;
		}
		ISocketSubsystem* const SocketSubSystem = ISocketSubsystem::Get();
		FString Ip;
		FAddressInfoResult AddrResult = SocketSubSystem->GetAddressInfo(*Domain, nullptr, EAddressInfoFlags::Default,
		                                                                NAME_None);
		if (AddrResult.ReturnCode == SE_NO_ERROR)
		{
			if (AddrResult.Results.Num() > 0)
			{
				Ip = AddrResult.Results[0].Address->ToString(false);
			}
		}
		else
		{
			Url = OldUrl;
			return false;
		}
		if (Port.IsEmpty())
		{
			Url = StartPrefix + Ip;
		}
		else
		{
			Url = StartPrefix + Ip + TEXT(":") + Port;
		}
		return true;
	};
	if (!GetIpFromDomain(TEXT("http://")))
	{
		if (!GetIpFromDomain(TEXT("https://")))
		{
		}
	}
	CURL* Curl = curl_easy_init();
	curl_slist* CurlHeaders = nullptr;

	curl_easy_setopt(Curl, CURLOPT_URL,TCHAR_TO_ANSI(*Url));
	curl_easy_setopt(Curl, CURLOPT_POST, 1L);
	curl_easy_setopt(Curl, CURLOPT_POSTFIELDS, nullptr);
	curl_easy_setopt(Curl, CURLOPT_POSTFIELDSIZE, Content.Len());
	curl_easy_setopt(Curl, CURLOPT_TIMEOUT_MS, 2000);
	curl_easy_setopt(Curl, CURLOPT_READDATA, &Content);
	curl_easy_setopt(Curl, CURLOPT_READFUNCTION, StaticReadFn);

	TArray<FString> Headers;
	Headers.Push(TEXT("Content-Type: application/json; charset=utf-8"));
	Headers.Push(FString::Printf(TEXT("Content-Length: %d"), Content.Len()));
	Headers.Push(FString::Printf(TEXT("Platform: %s"), *Platform));
	for (const FString& Header : Headers)
	{
		CurlHeaders = curl_slist_append(CurlHeaders, TCHAR_TO_ANSI(*Header));
	}
	curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, CurlHeaders);

	auto CurlResult = curl_easy_perform(Curl);

	long ResponseCode = 0;
	if (CURLE_OK == curl_easy_getinfo(Curl, CURLINFO_RESPONSE_CODE, &ResponseCode))
	{
	}
	FPlatformMisc::LowLevelOutputDebugStringf(
		TEXT("SendHttpSync %s, %d, %d"), *Url, (int)CurlResult, (int)ResponseCode);
	curl_slist_free_all(CurlHeaders);
	curl_easy_cleanup(Curl);
}
#else
static void SendHttpSync(const FString& Platform, const FString& Url, const FString& Content)
{
}
#endif
