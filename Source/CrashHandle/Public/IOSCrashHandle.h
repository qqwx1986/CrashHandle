#pragma once
#if PLATFORM_IOS
#incldue "IOS/IOSPlatformCrashContext.h"
class FIOSCrashHandle
{
public:
	static void DefaultCrashHandler(const FGenericCrashContext& Context);
};
#endif