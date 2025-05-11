#ifndef AUXILIARY_HEADER_SCOPE_DEFINING
#   error "this file can only be included in <auxiliary/config/platform.h>"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <TargetConditionals.h>

#ifdef __cplusplus
}
#endif

/** Building for macOS. */
#ifndef TARGET_MACOS
#   if (TARGET_OS_OSX || TARGET_OS_MACCATALYST)
#       define TARGET_MACOS		1
#   endif
#endif

/** Building for iOS. */
#ifndef TARGET_IOS
#   if TARGET_OS_IOS && !TARGET_OS_MACCATALYST
#       define TARGET_IOS		1
#   endif
#endif

/** Building for iOS on Mac Catalyst. */
#ifndef TARGET_MACCAT
#   if TARGET_OS_MACCATALYST
#       define TARGET_MACCAT	1
#   endif
#endif

/** Building for tvOS. */
#ifndef TARGET_TVOS
#   if TARGET_OS_TV
#       define TARGET_TVOS		1
#   endif
#endif

/** Building for iOS or tvOS. */
#ifndef TARGET_IOS_OR_TVOS
#   if defined(TARGET_IOS) || defined(TARGET_TVOS)
#       define TARGET_IOS_OR_TVOS	1
#   endif
#endif

/** Building for macOS or iOS. */
#ifndef TARGET_MACOS_OR_IOS
#   if defined(TARGET_MACOS) || defined(TARGET_IOS)
#       define TARGET_MACOS_OR_IOS	1
#   endif
#endif

/** Building for a Simulator. */
#ifndef TARGET_OS_SIMULATOR
#   if TARGET_OS_SIMULATOR
#       define TARGET_OS_SIMULATOR	1
#   endif
#endif

/** Building for Apple Silicon on iOS, tvOS, or macOS platform. */
#ifndef TARGET_APPLE_SILICON
#   if TARGET_CPU_ARM64
#       define TARGET_APPLE_SILICON	1
#   endif
#endif

/** Building for macOS with support for Apple Silicon. */
#ifndef TARGET_MACOS_APPLE_SILICON
#   if defined(TARGET_MACOS) && defined(TARGET_APPLE_SILICON)
#       define TARGET_MACOS_APPLE_SILICON	1
#   endif
#endif

/** Building with Xcode versions. */
#ifndef TARGET_XCODE_13
#   if ((__MAC_OS_X_VERSION_MAX_ALLOWED >= 120000) || (__IPHONE_OS_VERSION_MAX_ALLOWED >= 150000)) // Also covers tvOS
#       define TARGET_XCODE_13	1
#   endif
#endif

#ifndef TARGET_XCODE_12
#   if ((__MAC_OS_X_VERSION_MAX_ALLOWED >= 101600) || (__IPHONE_OS_VERSION_MAX_ALLOWED >= 140000)) // Also covers tvOS
#       define TARGET_XCODE_12	1
#   endif
#endif

#define OS_DPI 72
