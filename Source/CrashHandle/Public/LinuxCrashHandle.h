#pragma once
#if PLATFORM_LINUX
#include "Unix/UnixPlatformCrashContext.h"
class FLinuxCrashHandle
{
public:
	static void DefaultCrashHandler(const FGenericCrashContext& Context);
};
#endif