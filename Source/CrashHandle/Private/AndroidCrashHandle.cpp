#include "AndroidCrashHandle.h"
#if PLATFORM_ANDROID
#include "Android/AndroidPlatformStackWalk.h"
#include "Android/AndroidPlatformCrashContext.h"
#include "Misc/FeedbackContext.h"
#include "HttpReport.h"
#include "CrashHandle.h"

void FAndroidCrashHandle::DefaultCrashHandler(const FGenericCrashContext& _Context)
{
	const FAndroidCrashContext &Context = static_cast<const FAndroidCrashContext &>(_Context);
	static int32 bHasEntered = 0;
	if (FPlatformAtomics::InterlockedCompareExchange(&bHasEntered, 1, 0) == 0)
	{
		const SIZE_T StackTraceSize = 65535;
		ANSICHAR StackTrace[StackTraceSize];
		StackTrace[0] = 0;

		FPlatformMisc::LowLevelOutputDebugStringf(TEXT("Starting StackWalk..."));

		// Walk the stack and dump it to the allocated memory.
		FPlatformStackWalk::StackWalkAndDump(StackTrace, StackTraceSize, 0, Context.Context);
		UE_LOG(CrashHandle, Error, TEXT("\n%s\n"), ANSI_TO_TCHAR(StackTrace));

		if (GLog)
		{
			GLog->SetCurrentThreadAsMasterThread();
			GLog->Flush();
		}

		if (GWarn)
		{
			GWarn->Flush();
		}
		FHttpReport::Reporter(FHttpReport::Android,TEXT(""),FString(ANSI_TO_TCHAR(StackTrace)));

	}
}
#endif
