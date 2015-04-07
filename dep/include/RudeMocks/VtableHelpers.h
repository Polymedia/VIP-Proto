/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksVtableHelpers_INCLUDED
#define RudeMocksVtableHelpers_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/strong_typedef.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  Generic typedef for the raw address of a function in memory. This
    will typically be a 32-bit or 64-bit pointer. */

typedef void* FunctionAddress;

/////////////////////////////////////////////////////////////////////////////
/*  Vtable-related typedefs. */

typedef FunctionAddress* Vtable;            // a vtable is an array of function addresses
typedef Vtable* VtablePtr;

BOOST_STRONG_TYPEDEF(boost::uint32_t, VtableIndex)
static const VtableIndex InvalidVtableIndex = VtableIndex(static_cast<boost::uint32_t>(-1));

/////////////////////////////////////////////////////////////////////////////
/*  Returns the address of the vtable pointer at the given offset for
    the given object. */

template <typename T>
VtablePtr* GetVtablePtrAddress(T* object, std::size_t offset = 0)
{
    RUDE_MOCKS_ASSERT(offset < sizeof(T));
    return reinterpret_cast<VtablePtr*>(reinterpret_cast<boost::uint8_t*>(object) + offset);
}

/////////////////////////////////////////////////////////////////////////////

template <typename T>
const VtablePtr* GetVtablePtrAddress(const T* object, std::size_t offset = 0)
{
    RUDE_MOCKS_ASSERT(offset < sizeof(T));
    return reinterpret_cast<const VtablePtr*>(reinterpret_cast<boost::uint8_t*>(object) + offset);
}

/////////////////////////////////////////////////////////////////////////////
/*  Returns the vtable pointer at the given offset for the given object. */

template <typename T>
VtablePtr GetVtablePtr(T* object, std::size_t offset = 0)
{
    return reinterpret_cast<VtablePtr>(*GetVtablePtrAddress(object, offset));
}

/////////////////////////////////////////////////////////////////////////////

template <typename T>
const VtablePtr GetVtablePtr(const T* object, std::size_t offset = 0)
{
    return reinterpret_cast<const VtablePtr>(*GetVtablePtrAddress(object, offset));
}

/////////////////////////////////////////////////////////////////////////////

template <typename T>
void SetVtablePtr(T* object, VtablePtr vtable, std::size_t offset = 0)
{
    *GetVtablePtrAddress(object, offset) = vtable;
}

}   // namespace RudeMocks

#endif  // RudeMocksVtableHelpers_INCLUDED
