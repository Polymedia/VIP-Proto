/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockFunctionKey_INCLUDED
#define RudeMocksMockFunctionKey_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/VtableHelpers.h>
#include <boost/operators.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  This data class is a per-class, unique key for a mocked function. In 
    the case of a virtual member function it stores the vtable index
    of the function in the class' vtable. Additionally, the this offset
    in bytes is stored from the start of the object to the this pointer
    of the subobject for which the vtable index is valid. This, of course,
    only applies to classes that use multiple inheritance.
    
    In the case of a statically bound member function, a free function, or
    a static member function it stores the actual function address. */

class MockFunctionKey : public boost::totally_ordered<MockFunctionKey>
{
public:
    MockFunctionKey()
        :   m_vtableIndex(InvalidVtableIndex), m_thisAdjustment(0u), m_functionAddress(NULL)
    {}

    MockFunctionKey(VtableIndex vtableIndex, std::size_t thisAdjustment)
        :   m_vtableIndex(vtableIndex), m_thisAdjustment(thisAdjustment), m_functionAddress(NULL)
    {
        RUDE_MOCKS_ASSERT(InvalidVtableIndex != vtableIndex);
    }

    explicit MockFunctionKey(FunctionAddress functionAddress, std::size_t thisAdjustment = 0u)
        :   m_vtableIndex(InvalidVtableIndex), m_thisAdjustment(thisAdjustment), m_functionAddress(functionAddress)
    {
        RUDE_MOCKS_ASSERT(functionAddress);
    }

    VtableIndex GetVtableIndex() const { return m_vtableIndex; }
    std::size_t GetThisAdjustment() const { return m_thisAdjustment; }
    FunctionAddress GetFunctionAddress() const { return m_functionAddress; }

    bool IsValid() const
    {
        return GetVtableIndex() != InvalidVtableIndex || GetFunctionAddress() != NULL;
    }

    bool IsVirtual() const
    {
        return GetVtableIndex() != InvalidVtableIndex && GetFunctionAddress() == NULL;
    }

    bool operator < (const MockFunctionKey& other) const
    {
        return GetVtableIndex() < other.GetVtableIndex() ||
            (GetVtableIndex() == other.GetVtableIndex() && GetThisAdjustment() < other.GetThisAdjustment()) ||
            (GetThisAdjustment() == other.GetThisAdjustment() && GetFunctionAddress() < other.GetFunctionAddress());
    }

    bool operator == (const MockFunctionKey& other) const
    {
        return 
            GetVtableIndex() == other.GetVtableIndex() && 
            GetThisAdjustment() == other.GetThisAdjustment() &&
            GetFunctionAddress() == other.GetFunctionAddress();
    }

private:
    // only used for virtual member functions
    VtableIndex m_vtableIndex;
    std::size_t m_thisAdjustment;       // only != 0 in case of multiple inheritance

    // only used for non-virtual (i.e. statically bound) member functions or free functions
    FunctionAddress m_functionAddress;
};

}   // namespace RudeMocks

#endif  // RudeMocksMockFunctionKey_INCLUDED
