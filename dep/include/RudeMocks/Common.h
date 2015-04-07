/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksCommon_INCLUDED
#define RudeMocksCommon_INCLUDED

#include <RudeMocks/Config.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#if RUDE_MOCKS_USE_PRAGMA_COMMENT_LIB
    #ifdef RUDE_MOCKS_USE_COMPILER_HOOKS
        #ifdef _DEBUG
            #pragma comment(lib, "RudeMocks-d-hooks.lib")
        #else
            #pragma comment(lib, "RudeMocks-hooks.lib")
        #endif
    #else
        #ifdef _DEBUG
            #pragma comment(lib, "RudeMocks-d.lib")
        #else
            #pragma comment(lib, "RudeMocks.lib")
        #endif
    #endif
#endif

/////////////////////////////////////////////////////////////////////////////
/*  Include a number of common headers that we use throughout and don't want
    to include explicitly every time. This list should be kept as small
    as possible. */

#include <new>
#include <cassert>
#include <boost/cstdint.hpp>
#include <boost/concept_check.hpp>
#include <boost/static_assert.hpp>

/////////////////////////////////////////////////////////////////////////////
/*  A shared_ptr null deleter. */

struct NullDeleter
{
    void operator () (void*) const {}
};

/////////////////////////////////////////////////////////////////////////////
// Define the assert macros used by RudeMocks

#define RUDE_MOCKS_ASSERT(expr)            assert(expr)
#define RUDE_MOCKS_ASSERT_MSG(expr, msg)   assert((expr) && (msg))
#define RUDE_MOCKS_STATIC_ASSERT           BOOST_STATIC_ASSERT

#ifdef _DEBUG
#define RUDE_MOCKS_VERIFY(expr)            assert(expr)
#else
#define RUDE_MOCKS_VERIFY(expr)            ((void)(expr))
#endif

#endif	// RudeMocksCommon_INCLUDED

