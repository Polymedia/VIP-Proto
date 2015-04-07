/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockedObject_INCLUDED
#define RudeMocksMockedObject_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/VtableHelpers.h>
#include <RudeMocks/PlatformSpecific/MockFunctionThunks.h>
#include <RudeMocks/MockedObjectRegistry.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <vector>
#include <map>

namespace RudeMocks
{

class ClassMockBase;
template <typename T>
class ClassMock;

/////////////////////////////////////////////////////////////////////////////
/*  This internal class represent actual mocked objects. Instances of
    it are returned from MockRepository::CreateClassMock cast to the type
    of the mocked object. It should never be used by clients directly. */

class MockedObjectBase
{
protected:
    explicit MockedObjectBase(ClassMockBase* mock);
    virtual ~MockedObjectBase();

public:
    boost::shared_ptr<ClassMockBase> GetMock() const;
    ClassMockBase* GetRawMock() const { return m_mock; }       // necessary for the mock thunks

    std::size_t GetThisOffset(const void* adjustedThis) const;

    const boost::uint8_t* GetBeginAddress() const { return GetMockedObjectAddress(); }
    boost::uint8_t* GetBeginAddress() { return GetMockedObjectAddress(); }
    const boost::uint8_t* GetEndAddress() const { return GetMockedObjectAddress() + GetMockedObjectSize(); }
    boost::uint8_t* GetEndAddress() { return GetMockedObjectAddress() + GetMockedObjectSize(); }

    virtual void RegisterThisPointer(std::size_t offset) = 0;

protected:
    virtual boost::uint8_t* GetMockedObjectAddress() const = 0;
    virtual std::size_t GetMockedObjectSize() const = 0;

private:
    ClassMockBase* m_mock;                          // needs to store a raw pointer instead of a weak_ptr for the mock function thunks
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class MockedObject : public MockedObjectBase
{
protected:
    explicit MockedObject(ClassMock<T>* mock)
        :   MockedObjectBase(mock)
    {}

public:
    typedef T ObjectType;

    const T* GetMockedObjectPointer() const { return reinterpret_cast<const T*>(GetBeginAddress()); }
    T* GetMockedObjectPointer() { return reinterpret_cast<T*>(GetBeginAddress()); }

protected:
    virtual std::size_t GetMockedObjectSize() const { return sizeof(T); }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class FullyMockedObject : public MockedObject<T>
{
public:
    explicit FullyMockedObject(ClassMock<T>* mock)
        :   MockedObject<T>(mock),
            m_mockedObjectData(std::max(1u, sizeof(T)), 0xcd)        // ensure we have at least one byte in case of empty class optimization
    {}

    virtual void RegisterThisPointer(std::size_t offset)
    {
        // only patch the vtable if we can
        if (offset + sizeof(VtablePtr) <= sizeof(T))
            SetVtablePtr(MockedObject<T>::GetMockedObjectPointer(), PlatformSpecific::GetMockFunctionThunkVtable(), offset);
    }

protected:
    virtual boost::uint8_t* GetMockedObjectAddress() const { return const_cast<boost::uint8_t*>(&m_mockedObjectData[0]); }

private:
    std::vector<boost::uint8_t> m_mockedObjectData; // has sizeof(T) bytes; a pointer to this data block is returned as the mocked object
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class OriginalBasedMockedObject : public MockedObject<T>
{
public:
    OriginalBasedMockedObject(ClassMock<T>* mock, T& originalObject)
        :   MockedObject<T>(mock), m_originalObject(&originalObject)
    {}
    virtual ~OriginalBasedMockedObject()
    {
        RestoreOriginalVtablePointers();
    }

    const T* GetOriginalObjectPointer() const { return m_originalObject; }
    T* GetOriginalObjectPointer() { return m_originalObject; }

    virtual void RegisterThisPointer(std::size_t offset)
    {
        // only patch the vtable if we can
        if (offset + sizeof(VtablePtr) <= sizeof(T))
        {
            VtablePtr vtablePtr = GetVtablePtr(MockedObject<T>::GetMockedObjectPointer(), offset);
            m_originalVtablePointers.insert(std::make_pair(offset, vtablePtr));
            SetVtablePtr(MockedObject<T>::GetMockedObjectPointer(), PlatformSpecific::GetMockFunctionThunkVtable(), offset);
        }
    }

protected:
    virtual boost::uint8_t* GetMockedObjectAddress() const { return reinterpret_cast<boost::uint8_t*>(m_originalObject); }

private:
    void RestoreOriginalVtablePointers()
    {
        for (OffsetToVtablePtrMap::const_iterator it = m_originalVtablePointers.begin(); it != m_originalVtablePointers.end(); ++it)
        {
            std::size_t offset = it->first;
            VtablePtr vtablePtr = it->second;
            SetVtablePtr(MockedObject<T>::GetMockedObjectPointer(), vtablePtr, offset);
        }
    }

private:
    T* m_originalObject;

    typedef std::map<std::size_t, VtablePtr> OffsetToVtablePtrMap;
    OffsetToVtablePtrMap m_originalVtablePointers;
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class MockedObjectFactory
{
private:
    // A deleter for mocked objects that unregisters the mocked object with the MockedObjectRegistry.
    class MockedObjectDeleter
    {
    public:
        void operator () (MockedObject<T>* mockedObject) const
        {
            if (mockedObject)
            {
                MockedObjectRegistry::GetInstance().Unregister(mockedObject);
                boost::checked_delete(mockedObject);
            }
        }
    };

public:
    boost::shared_ptr<MockedObject<T> > Create(ClassMock<T>* mock)
    {
        RUDE_MOCKS_ASSERT(mock);
        boost::shared_ptr<MockedObject<T> > mockedObject(new FullyMockedObject<T>(mock), MockedObjectDeleter());
        MockedObjectRegistry::GetInstance().Register(mockedObject.get());
        return mockedObject;
    }

    boost::shared_ptr<MockedObject<T> > Create(ClassMock<T>* mock, T& originalObject)
    {
        RUDE_MOCKS_ASSERT(mock);
        boost::shared_ptr<MockedObject<T> > mockedObject(new OriginalBasedMockedObject<T>(mock, originalObject), MockedObjectDeleter());
        MockedObjectRegistry::GetInstance().Register(mockedObject.get());
        return mockedObject;
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksMockedObject_INCLUDED
