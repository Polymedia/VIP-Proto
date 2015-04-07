/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksFunctionFactory_INCLUDED
#define RudeMocksFunctionFactory_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/MockFunction.h>
#include <RudeMocks/MockedObject.h>
#include <RudeMocks/PatchedFunctionRegistry.h>
#include <RudeMocks/PlatformSpecific/FunctionPointerAnalyzer.h>
#include <RudeMocks/PlatformSpecific/MockFunctionThunks.h>
#include <boost/type_traits/is_base_of.hpp>

namespace RudeMocks
{

template <typename T>
class ClassMock;
class FreeFunctionMock;
class FunctionPatcher;

/////////////////////////////////////////////////////////////////////////////
/*  A factory class for creating mock function objects from function pointers,
    i.e. for creating objects that represent a particular function, its arity, 
    argument and return types, etc. 
    
    This factory assigns a per-mock, unique key to each function. For
    virtual member functions this key is the vtable index of the function
    in the class it belongs to. For statically bound functions it is 
    the start address of the function in memory. */

class FunctionFactory
{
private:
#if defined(RUDE_MOCKS_USE_COMPILER_HOOKS)
    template <typename ObjectT, typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> CreateMockMemberFunction(const boost::shared_ptr<ClassMock<ObjectT> >& mock, const MockFunctionKey& key) const
    {
        typedef FunctionSignature<FunctionPtrT> FunctionSignatureType;
        return boost::shared_ptr<MockFunctionBase>(new MockFunction<FunctionSignatureType>(mock, key));
    }

    template <typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> CreateMockFreeFunction(const boost::shared_ptr<FreeFunctionMock>& mock, const MockFunctionKey& key) const
    {
        typedef FunctionSignature<FunctionPtrT> FunctionSignatureType;
        return boost::shared_ptr<MockFunctionBase>(new MockFunction<FunctionSignatureType>(mock, key));
    }
#else   // defined(RUDE_MOCKS_USE_COMPILER_HOOKS)
    // A special deleter used for statically bound member functions and free functions, i.e. functions
    // that need to be patched. It keeps a reference to the function patcher that patched the function.
    class MockFunctionDeleter
    {
    public:
        explicit MockFunctionDeleter(const boost::shared_ptr<FunctionPatcher>& functionPatcher)
            :   m_functionPatcher(functionPatcher)
        {}

        void operator () (MockFunctionBase* function) const
        {
            if (function)
                delete function;
        }

    private:
        boost::shared_ptr<FunctionPatcher> m_functionPatcher;
    };

    template <typename ObjectT, typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> CreateMockMemberFunction(const boost::shared_ptr<ClassMock<ObjectT> >& mock, const MockFunctionKey& key) const
    {
        typedef FunctionSignature<FunctionPtrT> FunctionSignatureType;
        if (!key.IsVirtual())
        {
            boost::shared_ptr<FunctionPatcher> functionPatcher = PatchedFunctionRegistry::GetInstance().PatchFunction(
                key.GetFunctionAddress(), PlatformSpecific::GetStaticallyBoundMemberFunctionThunk());
            return boost::shared_ptr<MockFunctionBase>(new MockFunction<FunctionSignatureType>(mock, key), MockFunctionDeleter(functionPatcher));
        }
        else
            return boost::shared_ptr<MockFunctionBase>(new MockFunction<FunctionSignatureType>(mock, key));
    }

    template <typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> CreateMockFreeFunction(const boost::shared_ptr<FreeFunctionMock>& mock, const MockFunctionKey& key) const
    {
        RUDE_MOCKS_ASSERT_MSG(!PatchedFunctionRegistry::GetInstance().IsPatched(key.GetFunctionAddress()), "A free function can only be registered in one free function mock!");
        boost::shared_ptr<FunctionPatcher> functionPatcher = PatchedFunctionRegistry::GetInstance().PatchFunction(key.GetFunctionAddress(), 
            PlatformSpecific::GetFreeFunctionThunk());
        typedef FunctionSignature<FunctionPtrT> FunctionSignatureType;
        return boost::shared_ptr<MockFunctionBase>(new MockFunction<FunctionSignatureType>(mock, key), MockFunctionDeleter(functionPatcher));
    }
#endif  // defined(RUDE_MOCKS_USE_COMPILER_HOOKS)

public:
    template <typename ObjectT, typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> Create(const boost::shared_ptr<ClassMock<ObjectT> >& mock, 
        typename FunctionSignature<FunctionPtrT>::ObjectType* potentialMockedObjectBase, FunctionPtrT functionPtr) const
    {
        RUDE_MOCKS_ASSERT(mock);
        RUDE_MOCKS_ASSERT(potentialMockedObjectBase);
        RUDE_MOCKS_ASSERT(functionPtr);

        MockFunctionKey key = GetMemberFunctionKey(mock, potentialMockedObjectBase, functionPtr);
        RUDE_MOCKS_ASSERT(key.IsValid());

        const ObjectT* mockedObject = mock->GetMockedObject()->GetMockedObjectPointer();
        PlatformSpecific::MemberFunctionPointerAnalyzer<ObjectT, FunctionPtrT> analyzer(mockedObject, potentialMockedObjectBase, functionPtr);
        mock->GetMockedObject()->RegisterThisPointer(analyzer.GetThisAdjustment());

        return CreateMockMemberFunction<ObjectT, FunctionPtrT>(mock, key);
    }

    template <typename FunctionPtrT>
    boost::shared_ptr<MockFunctionBase> Create(const boost::shared_ptr<FreeFunctionMock>& mock, FunctionPtrT functionPtr) const
    {
        RUDE_MOCKS_STATIC_ASSERT(sizeof(functionPtr) == sizeof(FunctionAddress));       // should always be the case for free functions

        RUDE_MOCKS_ASSERT(mock);
        RUDE_MOCKS_ASSERT(functionPtr);

        MockFunctionKey key = GetFreeFunctionKey(functionPtr);
        RUDE_MOCKS_ASSERT(key.IsValid());
        RUDE_MOCKS_ASSERT(!key.IsVirtual());

        return CreateMockFreeFunction<FunctionPtrT>(mock, key);
    }

    template <typename ObjectT, typename FunctionPtrT>
    MockFunctionKey GetMemberFunctionKey(const boost::shared_ptr<ClassMock<ObjectT> >& mock, 
        typename FunctionSignature<FunctionPtrT>::ObjectType* potentialMockedObjectBase, FunctionPtrT functionPtr) const
    {
        const ObjectT* mockedObject = mock->GetMockedObject()->GetMockedObjectPointer();
        PlatformSpecific::MemberFunctionPointerAnalyzer<ObjectT, FunctionPtrT> analyzer(mockedObject, potentialMockedObjectBase, functionPtr);

        if (analyzer.GetInfo().IsVirtual())
            return MockFunctionKey(analyzer.GetInfo().GetVtableIndex(), analyzer.GetThisAdjustment());
        else
            return MockFunctionKey(analyzer.GetInfo().GetFunctionAddress(), analyzer.GetThisAdjustment());
    }

    template <typename FunctionPtrT>
    MockFunctionKey GetFreeFunctionKey(FunctionPtrT functionPtr) const
    {
        RUDE_MOCKS_STATIC_ASSERT(sizeof(FunctionPtrT) == 4);
        PlatformSpecific::FreeFunctionPointerAnalyzer<FunctionPtrT> analyzer(functionPtr);
        return MockFunctionKey(analyzer.GetInfo().GetFunctionAddress());
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksFunctionFactory_INCLUDED

