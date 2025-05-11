#pragma once

#ifdef __cplusplus
#   define AUXILIARY_CONSTEXPR  constexpr
#   define AUXILIARY_INLINE     inline
#   define AUXILIARY_EXTERN_C   extern "C"
#   define AUXILIARY_NOEXCEPT   noexcept
#else
#   define AUXILIARY_CONSTEXPR  const
#   define AUXILIARY_INLINE
#   define AUXILIARY_EXTERN_C   extern
#   define AUXILIARY_NOEXCEPT
#endif

