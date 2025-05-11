#pragma once

#include "key_words.h"

#define AUXILIARY_HEADER_SCOPE_DEFINING

#ifdef AUXILIARY_DEBUG
#   include "debug/breakpoint.inc"
#else
#   define AUXILIARY_DEBUG_BREAK()
#endif

#ifdef _MSC_VER
#   include "debug/win.inc"
#else
#   define AUXILIARY_WARNING_WINDOW(message)    printf("Warning: %s\n", message);
#   define AUXILIARY_ERROR_WINDOW(message)      printf("Error: %s\n", message); AUXILIARY_DEBUG_BREAK();
#endif

#undef AUXILIARY_HEADER_SCOPE_DEFINING
