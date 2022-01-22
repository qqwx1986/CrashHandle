#include "HttpReport.h"

#include "CrashHandleSetting.h"
#include "HttpModule.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
static void SendHttpSync(const FString& Platform, const FString& Url, const FString& Content);

UCrashHandleSetting* FHttpReport::Setting;

void FHttpReport::Reporter(FPlatformType Type, const FString& LogFile, const FString& DumpInfo)
{
	UE_LOG(LogTemp, Log, TEXT("Http Reporter1"));

	FString PlatformName = TEXT("Windows");
	switch (Type) {
		case Android:
			PlatformName = TEXT("Android");
			break;
		case IOS:
			PlatformName = TEXT("IOS");
			break;
		case Linux:
			PlatformName = TEXT("Linux");
			break;
		default:
			break;
	}
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString        CrashLogFile = FPaths::ProjectLogDir() / TEXT("Crash.log");
	PlatformFile.CopyFile(*CrashLogFile, *LogFile);
	FString LogInfo;
	FFileHelper::LoadFileToString(LogInfo, *CrashLogFile);

	TSharedRef<FJsonObject>   Serializer = MakeShareable(new FJsonObject);
	FString                   Content;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&Content);
	Serializer->SetStringField(TEXT("Platform"), PlatformName);
	Serializer->SetStringField(TEXT("Dump"), DumpInfo);
	Serializer->SetStringField(TEXT("Log"), LogInfo.Right(500));
	FJsonSerializer::Serialize(Serializer, JsonWriter);
	JsonWriter->Close();

	SendHttpSync(PlatformName, Setting->ServerUrl, Content);
}

static void DefaultCrashHandler(const FGenericCrashContext& Context)
{
	UE_LOG(LogTemp, Log, TEXT("DefaultCrashHandler"));
#if PLATFORM_IOS
#include "IOSCrashHandle.h"
	FIOSCrashHandle::DefaultCrashHandler(Context);
#endif
#if PLATFORM_ANDROID
#include "AndroidCrashHandle.h"
	UE_LOG(LogTemp,Log,TEXT("FAndroidCrashHandle"));
	FAndroidCrashHandle::DefaultCrashHandler(Context);
#endif
#if PLATFORM_LINUX
#include "LinuxCrashHandle.h"
	FLinuxCrashHandle::DefaultCrashHandler(Context);
#endif
}

void FHttpReport::RegisterCrashHandler()
{
	UE_LOG(LogTemp, Log, TEXT("RegisterCrashHandler"));
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

static void SendHttpSync(const FString& Platform, const FString& Url, const FString& Content)
{
	CURL*       Curl        = curl_easy_init();
	curl_slist* CurlHeaders = nullptr;

	curl_easy_setopt(Curl, CURLOPT_URL,TCHAR_TO_ANSI(*Url));
	curl_easy_setopt(Curl, CURLOPT_POST, 1L);
	curl_easy_setopt(Curl, CURLOPT_POSTFIELDS, nullptr);
	curl_easy_setopt(Curl, CURLOPT_POSTFIELDSIZE, Content.Len());
	curl_easy_setopt(Curl, CURLOPT_TIMEOUT_MS, 1000);
	curl_easy_setopt(Curl, CURLOPT_READDATA, &Content);
	curl_easy_setopt(Curl, CURLOPT_READFUNCTION, StaticReadFn);

	TArray<FString> Headers;
	Headers.Push(TEXT("Content-Type: application/json; charset=utf-8"));
	Headers.Push(FString::Printf(TEXT("Content-Length: %d"), Content.Len()));
	Headers.Push(FString::Printf(TEXT("Platform: %s"), *Platform));
	for (const FString& Header : Headers) {
		CurlHeaders = curl_slist_append(CurlHeaders, TCHAR_TO_ANSI(*Header));
	}
	curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, CurlHeaders);

	auto CurlResult = curl_easy_perform(Curl);

	long ResponseCode = 0;
	if (CURLE_OK == curl_easy_getinfo(Curl, CURLINFO_RESPONSE_CODE, &ResponseCode)) {
	}
	FPlatformMisc::LowLevelOutputDebugStringf(TEXT("SendHttpSync %d, %d"), (int)CurlResult, (int)ResponseCode);
	curl_slist_free_all(CurlHeaders);
	curl_easy_cleanup(Curl);
}
#endif
