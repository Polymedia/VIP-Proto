/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMock_INCLUDED
#define RudeMocksMock_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/MockedObject.h>
#include <RudeMocks/FunctionFactory.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <vector>
#include <map>

namespace RudeMocks
{

class ExpectationBase;
class MockFunctionBase;
class FunctionArgumentsBase;
class MockRepository;

/////////////////////////////////////////////////////////////////////////////
/*  The base record & replay interface for all mocks. */

class IMockRecordAndReplay
{
public:
    virtual ~IMockRecordAndReplay() {}

    virtual void Record(const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments) = 0;
    virtual boost::any Replay(const boost::shared_ptr<FunctionArgumentsBase>& functionArguments) = 0;
};

/////////////////////////////////////////////////////////////////////////////
/*  The mock base class that represents mocks for classes and functions.
    Clients should usually rarely ever have the need to call any of the
    public functions of this class, but instead interact with it using
    various free functions, such as ExpectCall. */

class Mock : public IMockRecordAndReplay, public boost::enable_shared_from_this<Mock>, private boost::noncopyable
{
public:
    enum IgnoreUnexpectedCallsMode
    {
        IgnoreUnexpectedCalls,
        DontIgnoreUnexpectedCalls
    };

protected:
    Mock(MockRepository& mockRepository, IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode)
        :   m_mockRepository(mockRepository), m_ignoreUnexpectedCallsMode(ignoreUnexpectedCallsMode)
    {}

public:
    virtual ~Mock();

    MockRepository& GetMockRepository() const { return m_mockRepository; }
    IgnoreUnexpectedCallsMode GetIgnoreUnexpectedCallsMode() const { return m_ignoreUnexpectedCallsMode; }
    void SetIgnoreUnexpectedCallsMode(IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode);

    const boost::shared_ptr<MockFunctionBase>& GetLastCalledFunction() const { return m_lastCalledFunction; }
    std::size_t GetFunctionCount() const { return m_functions.size(); }
    boost::shared_ptr<MockFunctionBase> FindFunction(MockFunctionKey key) const;
    bool HasFunction(MockFunctionKey key) const { return FindFunction(key); }

    RecordOrReplayFunction GetRecordOrReplayFunction(MockFunctionKey key);
    virtual void Record(const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments);
    virtual boost::any Replay(const boost::shared_ptr<FunctionArgumentsBase>& functionArguments);

protected:
    void RegisterFunction(const boost::shared_ptr<MockFunctionBase>& function);

private:
    MockRepository& m_mockRepository;
    IgnoreUnexpectedCallsMode m_ignoreUnexpectedCallsMode;

    boost::shared_ptr<MockFunctionBase> m_lastCalledFunction;
    
    typedef std::map<MockFunctionKey, boost::shared_ptr<MockFunctionBase> > FunctionMap;
    FunctionMap m_functions;
};

/////////////////////////////////////////////////////////////////////////////
/*  The base class for all class mocks. */

class ClassMockBase : public Mock
{
protected:
    ClassMockBase(MockRepository& mockRepository, IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode)
        :   Mock(mockRepository, ignoreUnexpectedCallsMode)
    {}
};

/////////////////////////////////////////////////////////////////////////////
/*  This mock class is responsible for mocking class member functions. */

template <typename T>
class ClassMock : public ClassMockBase
{
private:
    friend class MockRepository;

    ClassMock(MockRepository& mockRepository, IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode)
        :   ClassMockBase(mockRepository, ignoreUnexpectedCallsMode)
    {
        MockedObjectFactory<T> factory;
        m_mockedObject = factory.Create(this);
    }

    ClassMock(MockRepository& mockRepository, IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode, T& originalObject)
        :   ClassMockBase(mockRepository, ignoreUnexpectedCallsMode)
    {
        MockedObjectFactory<T> factory;
        m_mockedObject = factory.Create(this, originalObject);
    }

public:
    const boost::shared_ptr<MockedObject<T> >& GetMockedObject() const { return m_mockedObject; }

    template <typename FunctionPtrT>
    void RegisterFunction(T* mockedObject, FunctionPtrT functionPtr)
    {
        RUDE_MOCKS_ASSERT(GetClassMockFromObject(mockedObject).get() == this);

        // We need a pointer to the actual object type of the given member function pointer, which might be a base type of T.
        // This is for multiple inheritance so that we can determine the required this pointer adjustment when calling the function.
        typedef typename FunctionSignature<FunctionPtrT>::ObjectType FunctionSignatureObjectType;     // this also tests that FunctionPtrT is a member function pointer because otherwise it wouldn't compile
        RUDE_MOCKS_STATIC_ASSERT((boost::is_base_of<FunctionSignatureObjectType, T>::value));

        // If you get a compilation error in the next line then you need to use RegisterAmbiguousFunction instead of this function
        // and pass in an explicitly upcasted pointer to the mocked object.
        FunctionSignatureObjectType* potentialMockedObjectBase = static_cast<FunctionSignatureObjectType*>(mockedObject);

        FunctionFactory functionFactory;
        boost::shared_ptr<MockFunctionBase> function = functionFactory.Create<T>(GetClassMockFromObject(mockedObject), potentialMockedObjectBase, functionPtr);
        Mock::RegisterFunction(function);
    }

    template <typename FunctionPtrT>
    void RegisterAmbiguousFunction(T* mockedObject, typename FunctionSignature<FunctionPtrT>::ObjectType* potentialMockedObjectBase, FunctionPtrT functionPtr)
    {
        RUDE_MOCKS_ASSERT(GetClassMockFromObject(mockedObject).get() == this);

        FunctionFactory functionFactory;
        boost::shared_ptr<MockFunctionBase> function = functionFactory.Create<T>(GetClassMockFromObject(mockedObject), potentialMockedObjectBase, functionPtr);
        Mock::RegisterFunction(function);
    }

private:
    boost::shared_ptr<MockedObject<T> > m_mockedObject;
};

/////////////////////////////////////////////////////////////////////////////
/*  This is the mock class for free functions and static member functions. */

class FreeFunctionMock : public Mock
{
private:
    friend class MockRepository;

    FreeFunctionMock(MockRepository& mockRepository, IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode)
        :   Mock(mockRepository, ignoreUnexpectedCallsMode)
    {}

public:
    template <typename FunctionPtrT>
    void RegisterFunction(FunctionPtrT functionPtr)
    {
        FunctionFactory functionFactory;
        boost::shared_ptr<MockFunctionBase> function = functionFactory.Create(boost::static_pointer_cast<FreeFunctionMock>(shared_from_this()), functionPtr);
        Mock::RegisterFunction(function);
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksMock_INCLUDED
