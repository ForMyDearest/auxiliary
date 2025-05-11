#ifndef AUXILIARY_HEADER_SCOPE_DEFINING
#   error "this file can only be included in <auxiliary/config/debug.h>"
#endif

#if defined(_MSC_VER)
#   define AUXILIARY_DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
#   define AUXILIARY_DEBUG_BREAK() __builtin_trap()
#else
#   define AUXILIARY_DEBUG_BREAK()
#endif
