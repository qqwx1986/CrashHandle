#pragma once
#if PLATFORM_IOS
#include "GenericPlatform/GenericPlatformCrashContext.h"
class FIOSCrashHandle
{
public:
	static void DefaultCrashHandler(const FGenericCrashContext& Context);
};
#endif