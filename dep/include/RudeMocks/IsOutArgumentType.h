/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksIsOutArgumentType_INCLUDED
#define RudeMocksIsOutArgumentType_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/mpl/bool.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A template metafunction that returns true if the given type is
    an out argument type, i.e. it needs to be non-const T& or T*. */

template <typename T>
struct IsOutArgumentType : public boost::mpl::bool_<false>
{};

template <typename T>
struct IsOutArgumentType<T&> : public boost::mpl::bool_<true>
{};

template <typename T>
struct IsOutArgumentType<T*> : public boost::mpl::bool_<true>
{};

// const T& and const T* are not out parameters
template <typename T>
struct IsOutArgumentType<const T&> : public boost::mpl::bool_<false>
{};

template <typename T>
struct IsOutArgumentType<const T*> : public boost::mpl::bool_<false>
{};

}   // namespace RudeMocks

#endif  // RudeMocksIsOutArgumentType_INCLUDED
