#pragma once
#if PLATFORM_ANDROID

#include "GenericPlatform/GenericPlatformCrashContext.h"

class FAndroidCrashHandle
{
public:
	static void DefaultCrashHandler(const FGenericCrashContext& Context);
};
#endif