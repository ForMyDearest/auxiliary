#pragma once

/*
AUXILIARY_COMPILER_CLANG_CL
AUXILIARY_COMPILER_MSVC
AUXILIARY_COMPILER_GCC
AUXILIARY_COMPILER_CLANG
*/

#if defined(_MSC_VER)
#   if defined(__clang__)
#       define AUXILIARY_COMPILER_CLANG_CL	1
#   else
#       define AUXILIARY_COMPILER_MSVC		1
#   endif
#elif defined(__GNUC__)
#	define AUXILIARY_COMPILER_GCC		1
#elif defined(__clang__)
#	define AUXILIARY_COMPILER_CLANG		1
#else
#   error Unrecognized Compiler
#endif

#ifndef AUXILIARY_COMPILER_CLANG_CL
#	define AUXILIARY_COMPILER_CLANG_CL	0
#endif
#ifndef AUXILIARY_COMPILER_MSVC
#	define AUXILIARY_COMPILER_MSVC		0
#endif
#ifndef AUXILIARY_COMPILER_GCC
#	define AUXILIARY_COMPILER_GCC		0
#endif
#ifndef AUXILIARY_COMPILER_CLANG
#	define AUXILIARY_COMPILER_CLANG		0
#endif
