#include "IOSCrashHandle.h"
#if PLATFORM_IOS

void FIOSCrashHandle::DefaultCrashHandler(const FGenericCrashContext& Context)
{
	Context.ReportCrash();
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
	return Context.GenerateCrashInfo();
}

#endif