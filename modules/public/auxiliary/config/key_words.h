#pragma once

#include "auxiliary/config/compiler.h"
#include <cassert>

//== attribute =================================================================
#ifdef __has_cpp_attribute
#
//====== deprecated ============================================================
#   if __has_cpp_attribute(deprecated)
#       define AUXILIARY_DEPRECATED(msg)    [[deprecated(msg)]]
#   elif defined(__GNUC__) || defined(__clang__)
#       define AUXILIARY_DEPRECATED(msg)    __attribute__((deprecated))
#   elif defined(_MSC_VER)
#       define AUXILIARY_DEPRECATED(msg)    __declspec(deprecated)
#   else
#       define AUXILIARY_DEPRECATED(msg)
#   endif
//==============================================================================
#
//====== assume ================================================================
#   if __has_cpp_attribute(assume)
#       define AUXILIARY_ASSUME(expr)       [[assume(expr)]]
#   elif defined(__clang__)
#       define AUXILIARY_ASSUME(expr)       __builtin_assume(expr)
#   elif defined(_MSC_VER)
#       define AUXILIARY_ASSUME(expr)       __assume(expr)
#   elif defined(__GNUC__)
#       define AUXILIARY_ASSUME(expr)       __attribute__((assume(expr)))
#   else
#       define AUXILIARY_ASSUME(expr)
#   endif
//==============================================================================
#
//====== no unique address =====================================================
#   if __has_cpp_attribute(no_unique_address)
#       define AUXILIARY_NO_UNIQUE_ADDRESS  [[no_unique_address]]
#   elif __has_cpp_attribute(msvc::no_unique_address)
#       define AUXILIARY_NO_UNIQUE_ADDRESS  [[msvc::no_unique_address]]
#   else
#       define AUXILIARY_NO_UNIQUE_ADDRESS
#   endif
//==============================================================================
#endif
//==============================================================================


//== optimization ==============================================================
#ifdef _MSC_VER
#   define AUXILIARY_DISABLE_OPTIMIZATION   __pragma(optimize("", off))
#   define AUXILIARY_ENABLE_OPTIMIZATION    __pragma(optimize("", on))
#elif defined(__clang__)
#   define AUXILIARY_DISABLE_OPTIMIZATION   #pragma clang optimize off
#   define AUXILIARY_ENABLE_OPTIMIZATION    #pragma clang optimize on
#endif
//==============================================================================


//== MSVC ======================================================================
#ifdef _MSC_VER
#   define AUXILIARY_FORCEINLINE			__forceinline
#   define AUXILIARY_NOINLINE				__declspec(noinline)
#	define AUXILIARY_EBCO					__declspec(empty_bases)
#	define AUXILIARY_NOVTABLE				__declspec(novtable)
#	ifndef __MINGW32__
#		if (_MSC_VER >= 1900) && !defined(__EDG__)
#			define AUXILIARY_ALLOCATOR		__declspec(allocator)
#		else
#			define AUXILIARY_ALLOCATOR
#		endif
#		define AUXILIARY_RESTRICT			__declspec(restrict)
#	else
#		define AUXILIARY_RESTRICT
#	endif
#else
#   define AUXILIARY_FORCEINLINE			__inline__ __attribute__((always_inline))
#   define AUXILIARY_NOINLINE				__attribute__((noinline))
#	define AUXILIARY_EBCO
#	define AUXILIARY_NOVTABLE
#	define AUXILIARY_ALLOCATOR
#	define AUXILIARY_RESTRICT
#endif
//==============================================================================


//== debug =====================================================================
//         debug       releasedbg        release
//  vc    _DEBUG      _DEBUG,NDEBUG      NDEBUG
// gcc    _DEBUG         NDEBUG          NDEBUG

#if defined(__OPTIMIZE__)
#   ifndef NDEBUG
#       define NDEBUG
#   endif
#endif

#ifndef NDEBUG
#   define AUXILIARY_DEBUG    1
#   ifndef _DEBUG
#       define _DEBUG
#   endif
#else
#   define AUXILIARY_DEBUG    0
#endif
//==============================================================================


//== export ====================================================================
#ifdef AUXILIARY_DLL
#	ifndef AUXILIARY_API
#		ifdef _WIN32
#			define AUXILIARY_API __declspec(dllimport)
#		else
#			define AUXILIARY_API __attribute__((visibility("default")))
#		endif
#	endif
#else
#	define AUXILIARY_API
#endif
//==============================================================================
