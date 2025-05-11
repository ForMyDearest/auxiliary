#pragma once

//== CPU =======================================================================
#if defined(__x86_64__) || defined(_M_X64) || defined(_AMD64_) || defined(_M_AMD64)
#   define AUXILIARY_ARCH_X86_64
#   define AUXILIARY_ARCH_BIT_64
#elif defined(__i386) || defined(_M_IX86) || defined(_X86_)
#   define AUXILIARY_ARCH_X86
#   define AUXILIARY_ARCH_BIT_32
#elif defined(__aarch64__) || defined(__AARCH64) || defined(_M_ARM64)
#   define AUXILIARY_ARCH_ARM64
#   define AUXILIARY_ARCH_BIT_64
#elif defined(__arm__) || defined(_M_ARM)
#   define AUXILIARY_ARCH_ARM32
#   define AUXILIARY_ARCH_BIT_32
#elif defined(__POWERPC64__) || defined(__powerpc64__)
#   define AUXILIARY_ARCH_POWERPC64
#   define AUXILIARY_ARCH_BIT_64
#elif defined(__POWERPC__) || defined(__powerpc__)
#   define AUXILIARY_ARCH_POWERPC32
#   define AUXILIARY_ARCH_BIT_32
#elif defined(__wasm64__)
#   define AUXILIARY_ARCH_WASM64
#   define AUXILIARY_ARCH_WASM
#   define AUXILIARY_ARCH_BIT_64
#elif defined(__wasm__) || defined(__EMSCRIPTEN__) || defined(__wasi__)
#   define AUXILIARY_ARCH_WASM32
#   define AUXILIARY_ARCH_WASM
#   define AUXILIARY_ARCH_BIT_32
#else
#   error Unrecognized CPU
#endif
//==============================================================================