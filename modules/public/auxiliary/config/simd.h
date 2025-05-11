#pragma once

#if defined(__AVX512F__) && defined(__AVX512VL__) && defined(__AVX512DQ__)
#   define AUXILIARY_ARCH_AVX512
#endif
#if defined(AUXILIARY_ARCH_AVX512) || defined(__AVX2__)
#   define AUXILIARY_ARCH_AVX2
#endif
#if defined(AUXILIARY_ARCH_AVX2) || defined(__AVX__)
#   define AUXILIARY_ARCH_AVX
#endif
#if defined(AUXILIARY_ARCH_AVX) || defined(__SSE4_1__)
#   define AUXILIARY_ARCH_SSE4_1
#endif
