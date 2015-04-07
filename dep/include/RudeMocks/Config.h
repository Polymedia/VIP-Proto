/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksConfig_INCLUDED
#define RudeMocksConfig_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// Define macros for what compiler we're building with.

#if defined(_MSC_VER)
    #define RUDE_MOCKS_COMPILER_MSVC 1

    #if _MSC_VER == 1300
        #define RUDE_MOCKS_COMPILER_MAJOR_VERSION 7
        #define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
    #elif _MSC_VER == 1310
        #define RUDE_MOCKS_COMPILER_MAJOR_VERSION 7
        #define RUDE_MOCKS_COMPILER_MINOR_VERSION 1
	#elif _MSC_VER == 1400
		#define RUDE_MOCKS_COMPILER_MAJOR_VERSION 8
		#define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
    #elif _MSC_VER == 1500
        #define RUDE_MOCKS_COMPILER_MAJOR_VERSION 9
        #define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
    #elif _MSC_VER == 1600
        #define RUDE_MOCKS_COMPILER_MAJOR_VERSION 10
        #define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
    #elif _MSC_VER == 1700
        #define RUDE_MOCKS_COMPILER_MAJOR_VERSION 11
        #define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
    #endif
#elif defined(__GNUC__)
    #define RUDE_MOCKS_COMPILER_GCC 1

    #define RUDE_MOCKS_COMPILER_MAJOR_VERSION __GNUC__
    #define RUDE_MOCKS_COMPILER_MINOR_VERSION __GNUC_MINOR__
#endif

#ifndef RUDE_MOCKS_COMPILER_MAJOR_VERSION
#define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
#endif
#ifndef RUDE_MOCKS_COMPILER_MAJOR_VERSION
#define RUDE_MOCKS_COMPILER_MINOR_VERSION 0
#endif

/////////////////////////////////////////////////////////////////////////////
// Define a macro to detect which compiler and compiler version we have. */

#define RUDE_MOCKS_CHECK_COMPILER_VERSION(compiler, major, minor)       \
    (                                                                   \
        (compiler) == 1 &&                                              \
        (                                                               \
            RUDE_MOCKS_COMPILER_MAJOR_VERSION > (major) ||              \
            (                                                           \
                RUDE_MOCKS_COMPILER_MAJOR_VERSION == (major) &&         \
                RUDE_MOCKS_COMPILER_MINOR_VERSION >= (minor)            \
            )                                                           \
        )                                                               \
    )

#if !RUDE_MOCKS_CHECK_COMPILER_VERSION(RUDE_MOCKS_COMPILER_MSVC, 7, 1) && \
    !RUDE_MOCKS_CHECK_COMPILER_VERSION(RUDE_MOCKS_COMPILER_GCC, 3, 0)
    #error Building with unsupported compiler.
#endif

/////////////////////////////////////////////////////////////////////////////
// Make sure a processor architecture is defined.

#if defined(_X86_) || defined(_M_IX86)
    #ifndef RUDE_MOCKS_ARCH_X86
    #define RUDE_MOCKS_ARCH_X86 1
    #endif
#elif defined(_XBOX) || defined(XBOX)
    #ifndef RUDE_MOCKS_ARCH_XBOX360
    #define RUDE_MOCKS_ARCH_XBOX360 1
    #endif
#endif

#if !RUDE_MOCKS_ARCH_X86 && !RUDE_MOCKS_ARCH_XBOX360
    #error Building for unsupported processor architecture.
#endif

/////////////////////////////////////////////////////////////////////////////
// Make sure an operating system is defined.

#if defined(_XBOX) || defined(XBOX)         // needs to come first because _WIN32 is also defined for Xbox
    #ifndef RUDE_MOCKS_OS_XBOX360
    #define RUDE_MOCKS_OS_XBOX360 1
    #endif
#elif defined(_WIN32) || defined(WIN32) || defined(_WINDOWS)
    #ifndef RUDE_MOCKS_OS_WINDOWS
    #define RUDE_MOCKS_OS_WINDOWS 1
    #endif
#endif

#if !RUDE_MOCKS_OS_WINDOWS && !RUDE_MOCKS_OS_XBOX360
    #error Building for unsupported operating system.
#endif

#ifdef RUDE_MOCKS_ARCH_X86
#define RUDE_MOCKS_HAS_STDCALL 1
#define RUDE_MOCKS_HAS_FASTCALL 1
#endif

#ifdef RUDE_MOCKS_COMPILER_MSVC
#define RUDE_MOCKS_SUPPORTS_MEMBER_FUNCTION_CALLING_CONVENTIONS 1
#endif

/////////////////////////////////////////////////////////////////////////////
// Define a macro that tells us whether we should use #pragma once in headers.

#if RUDE_MOCKS_COMPILER_MSVC || RUDE_MOCKS_CHECK_COMPILER_VERSION(RUDE_MOCKS_COMPILER_GCC, 3, 4)
#define RUDE_MOCKS_USE_PRAGMA_ONCE 1
#endif

#if RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

/////////////////////////////////////////////////////////////////////////////
// Windows-specific configuration

#if RUDE_MOCKS_OS_WINDOWS
    #ifndef WINVER
    #define WINVER 0x500
    #endif
#endif

#if RUDE_MOCKS_OS_WINDOWS || RUDE_MOCKS_OS_XBOX360
    #ifndef STRICT
    #define STRICT
    #endif

    #ifndef NOMINMAX
    #define NOMINMAX                                // this causes windef.h to not define the min and max macro; use std::min and std::max instead
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////
// Configure Boost

#define BOOST_TYPEOF_SILENT
#define BOOST_AUTO_LINK_NOMANGLE

/////////////////////////////////////////////////////////////////////////////
// Compile-time limits

#define RUDE_MOCKS_MAX_FUNCTION_ARITY 10                    // the maximum number of arguments that can be used for mocked functions
#define RUDE_MOCKS_MAX_REGISTER_FUNCTIONS_ARITY 10			// the maximum number of function pointers that can be passed to RegisterFunctions
#define RUDE_MOCKS_MAX_VTABLE_THUNK_COUNT 40                // the maximum supported number of virtual functions of a class that can be mocked

/////////////////////////////////////////////////////////////////////////////
// Features

// Enables/disables auto-linking with the RudeMocks library with the 
// Microsoft Visual C++ compiler via #pragma comment(lib).
#if RUDE_MOCKS_COMPILER_MSVC
#ifndef RUDE_MOCKS_USE_PRAGMA_COMMENT_LIB
#define RUDE_MOCKS_USE_PRAGMA_COMMENT_LIB 1
#endif
#endif

// Enables/disables compiling in the BoostTestVerificationFailureHandler 
// class that is used with the Boost.Test library. Set this to 0 if you don't 
// intend on using Boost.Test.
#ifndef RUDE_MOCKS_USE_BOOST_TEST_VERIFICATION_FAILURE_HANDLER
#define RUDE_MOCKS_USE_BOOST_TEST_VERIFICATION_FAILURE_HANDLER 1
#endif

#endif  // RudeMocksConfig_INCLUDED


