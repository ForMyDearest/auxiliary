#pragma once

/*
AUXILIARY_HAS_CXX_11
AUXILIARY_HAS_CXX_14
AUXILIARY_HAS_CXX_17
AUXILIARY_HAS_CXX_20
*/

#if (__cplusplus >= 201103L)
#	define AUXILIARY_HAS_CXX_11 1
#elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#	define AUXILIARY_HAS_CXX_11 1
#elif defined(_MSC_VER) && (_MSC_VER >= 1600)
#	define AUXILIARY_HAS_CXX_11 1
#endif

#if (__cplusplus >= 201402L)
#	define AUXILIARY_HAS_CXX_14 1
#elif defined(_MSC_VER) && (_MSC_VER >= 1900)
#	define AUXILIARY_HAS_CXX_14 1
#endif

#if (__cplusplus >= 201703L)
#	define AUXILIARY_HAS_CXX_17 1
#elif defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)
#	define AUXILIARY_HAS_CXX_17 1
#endif

#if (__cplusplus >= 202002L)
#	define AUXILIARY_HAS_CXX_20 1
#elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202002L)
#	define AUXILIARY_HAS_CXX_20 1
#endif

#ifndef AUXILIARY_HAS_CXX_11
#	define AUXILIARY_HAS_CXX_11	0
#endif
#ifndef AUXILIARY_HAS_CXX_14
#	define AUXILIARY_HAS_CXX_14	0
#endif
#ifndef AUXILIARY_HAS_CXX_17
#	define AUXILIARY_HAS_CXX_17	0
#endif
#ifndef AUXILIARY_HAS_CXX_20
#	define AUXILIARY_HAS_CXX_20	0
#endif
