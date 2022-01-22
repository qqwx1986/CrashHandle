#include "LinuxCrashHandle.h"
#if PLATFORM_LINUX
#include "HAL/ThreadHeartBeat.h"

void FLinuxCrashHandle::DefaultCrashHandler(const FGenericCrashContext& Context)
{
	printf("DefaultCrashHandler: Signal=%d\n", Context.Signal);

	// Stop the heartbeat thread so that it doesn't interfere with crashreporting
	FThreadHeartBeat::Get().Stop();

	// at this point we should already be using malloc crash handler (see PlatformCrashHandler)
	const_cast<FUnixCrashContext&>(Context).CaptureStackTrace();
	if (GLog)
	{
		GLog->SetCurrentThreadAsMasterThread();
		GLog->Flush();
	}
	if (GWarn)
	{
		GWarn->Flush();
	}
	if (GError)
	{
		GError->Flush();
		GError->HandleError();
	}

	return Context.GenerateCrashInfoAndLaunchReporter();
}
#endif