/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockedObjectRegistry_INCLUDED
#define RudeMocksMockedObjectRegistry_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/noncopyable.hpp>
#include <set>

namespace RudeMocks
{

class MockedObjectBase;

/////////////////////////////////////////////////////////////////////////////
/*  A global registry of all MockedObject instances in the system.
    The registry maps adjusted this pointers to MockedObject instances.
    This is required for classes with multiple inheritance that can
    have multiple this pointers, depending on which subobject a function
    is called on. If a mock function thunk is called with such an
    adjusted this pointer it asks this registry to find the corresponding
    MockedObject instance. */

class MockedObjectRegistry : private boost::noncopyable
{
public:
    static MockedObjectRegistry& GetInstance();

    void Register(MockedObjectBase* mockedObject);
    void Unregister(MockedObjectBase* mockedObject);
    bool IsRegistered(MockedObjectBase* mockedObject) const;

    MockedObjectBase* GetMockedObjectFromAdjustedThis(void* adjustedThis) const;

private:
    typedef std::set<MockedObjectBase*> MockedObjectContainer;
    MockedObjectContainer m_mockedObjects;
};

}   // namespace RudeMocks

#endif  // RudeMocksMockedObjectRegistry_INCLUDED
