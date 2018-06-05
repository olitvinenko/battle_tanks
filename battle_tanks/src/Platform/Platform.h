#pragma once

#if defined(__ANDROID__) /* Android platform */
#define OS_ANDROID
#endif

#if defined(__APPLE__) && defined(__MACH__) /* Apple OSX and iOS (Darwin) */
	#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR == 1 /* iOS in Xcode simulator */
	#define OS_IOS_SIMULATOR
#elif TARGET_OS_IPHONE == 1 /* iOS on iPhone, iPad, etc. */
	#define OS_IOS
#elif TARGET_OS_MAC == 1 /* OS X */
	#define OS_MAC
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define OS_WINDOWS /* Microsoft Windows */
#endif
