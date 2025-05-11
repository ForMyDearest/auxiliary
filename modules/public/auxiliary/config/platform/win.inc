#ifndef AUXILIARY_HEADER_SCOPE_DEFINING
#   error "this file can only be included in <auxiliary/config/platform.h>"
#endif

#define AUXILIARY_PLATFORM_WINDOWS  1

#include <Windows.h>
#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
#   define AUXILIARY_PLATFORM_UWP   1
#endif

#define OS_DPI 96
