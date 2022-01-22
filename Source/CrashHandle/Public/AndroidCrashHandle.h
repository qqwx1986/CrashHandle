#pragma once
#if PLATFORM_ANDROID

#include "Android/AndroidPlatformCrashContext.h"

class FAndroidCrashHandle
{
public:
	static void DefaultCrashHandler(const FGenericCrashContext& Context);
};
#endif