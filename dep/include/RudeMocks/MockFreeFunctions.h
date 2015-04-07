/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockFreeFunctions_INCLUDED
#define RudeMocksMockFreeFunctions_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/Mock.h>
#include <RudeMocks/MockRepository.h>
#include <RudeMocks/MockedObjectRegistry.h>
#include <RudeMocks/FunctionOptions.h>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  Given a mocked object that was created via MockRepository::CreateClassMock
    returns the corresponding ClassMock object. */

template <typename ObjectT>
boost::shared_ptr<ClassMock<ObjectT> > GetClassMockFromObject(ObjectT* mockedObject)
{
    RUDE_MOCKS_ASSERT(mockedObject);
    MockedObjectBase* mockedObjectBase = MockedObjectRegistry::GetInstance().GetMockedObjectFromAdjustedThis(mockedObject);
    RUDE_MOCKS_ASSERT(mockedObjectBase);
    boost::shared_ptr<ClassMock<ObjectT> > mock = boost::dynamic_pointer_cast<ClassMock<ObjectT> >(mockedObjectBase->GetMock());
    RUDE_MOCKS_ASSERT(mock);
    return mock;
}

/////////////////////////////////////////////////////////////////////////////
/*  Returns a shared_ptr to the given free function mock. */

inline boost::shared_ptr<FreeFunctionMock> GetFreeFunctionMockFromObject(FreeFunctionMock* freeFunctionMock)
{
    return boost::static_pointer_cast<FreeFunctionMock>(freeFunctionMock->shared_from_this());
}

/////////////////////////////////////////////////////////////////////////////
/*  Create the RegisterFunctions free function for registering member
    functions in a class mock using preprocessor metaprogramming. */

#define RUDE_MOCKS_IMPLEMENT_REGISTER_MEMBER_FUNCTIONS(z, n, _) \
    GetClassMockFromObject(mockedObject)->RegisterFunction(mockedObject, BOOST_PP_CAT(functionPtr, n));

#define RUDE_MOCKS_DEFINE_REGISTER_MEMBER_FUNCTIONS(z, n, _) \
	template <typename ObjectT, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename FunctionPtrT)> \
	void RegisterFunctions(ObjectT* mockedObject, BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_INC(n), FunctionPtrT, functionPtr)) \
	{ \
		BOOST_PP_REPEAT(BOOST_PP_INC(n), RUDE_MOCKS_IMPLEMENT_REGISTER_MEMBER_FUNCTIONS, BOOST_PP_EMPTY()) \
	}

BOOST_PP_REPEAT(RUDE_MOCKS_MAX_REGISTER_FUNCTIONS_ARITY, RUDE_MOCKS_DEFINE_REGISTER_MEMBER_FUNCTIONS, BOOST_PP_EMPTY())

#undef RUDE_MOCKS_DEFINE_REGISTER_MEMBER_FUNCTIONS
#undef RUDE_MOCKS_IMPLEMENT_REGISTER_MEMBER_FUNCTIONS

/////////////////////////////////////////////////////////////////////////////
/*  Create the RegisterAmbiguousFunctions free function for registering member
    functions of ambiguous base classes in a class mock using preprocessor
    metaprogramming. */

#define RUDE_MOCKS_IMPLEMENT_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS(z, n, _) \
    { RUDE_MOCKS_STATIC_ASSERT((boost::is_base_of<typename FunctionSignature<BOOST_PP_CAT(FunctionPtrT, n)>::ObjectType, BaseObjectT>::value)); } \
    GetClassMockFromObject(mockedObject)->RegisterAmbiguousFunction(mockedObject, mockedObjectBase, BOOST_PP_CAT(functionPtr, n));

#define RUDE_MOCKS_DEFINE_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS(z, n, _) \
	template <typename ObjectT, typename BaseObjectT, BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename FunctionPtrT)> \
	void RegisterAmbiguousFunctions(ObjectT* mockedObject, BaseObjectT* mockedObjectBase, BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_INC(n), FunctionPtrT, functionPtr)) \
	{ \
        { RUDE_MOCKS_STATIC_ASSERT((boost::is_base_of<BaseObjectT, ObjectT>::value)); } \
		BOOST_PP_REPEAT(BOOST_PP_INC(n), RUDE_MOCKS_IMPLEMENT_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS, BOOST_PP_EMPTY()) \
	}

BOOST_PP_REPEAT(RUDE_MOCKS_MAX_REGISTER_FUNCTIONS_ARITY, RUDE_MOCKS_DEFINE_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS, BOOST_PP_EMPTY())

#undef RUDE_MOCKS_DEFINE_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS
#undef RUDE_MOCKS_IMPLEMENT_REGISTER_AMBIGUOUS_MEMBER_FUNCTIONS

/////////////////////////////////////////////////////////////////////////////
/*  Create the RegisterFunctions free function for registering free
    functions in a free function mock using preprocessor metaprogramming. */

#define RUDE_MOCKS_IMPLEMENT_REGISTER_FREE_FUNCTIONS(z, n, _) \
    mock->RegisterFunction(BOOST_PP_CAT(functionPtr, n));

#define RUDE_MOCKS_DEFINE_REGISTER_FREE_FUNCTIONS(z, n, _) \
	template <BOOST_PP_ENUM_PARAMS(BOOST_PP_INC(n), typename FunctionPtrT)> \
	void RegisterFunctions(FreeFunctionMock* mock, BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_INC(n), FunctionPtrT, functionPtr)) \
	{ \
        RUDE_MOCKS_ASSERT(mock); \
		BOOST_PP_REPEAT(BOOST_PP_INC(n), RUDE_MOCKS_IMPLEMENT_REGISTER_FREE_FUNCTIONS, BOOST_PP_EMPTY()) \
	}

BOOST_PP_REPEAT(RUDE_MOCKS_MAX_REGISTER_FUNCTIONS_ARITY, RUDE_MOCKS_DEFINE_REGISTER_FREE_FUNCTIONS, BOOST_PP_EMPTY())

#undef RUDE_MOCKS_DEFINE_REGISTER_FREE_FUNCTIONS
#undef RUDE_MOCKS_IMPLEMENT_REGISTER_FREE_FUNCTIONS

/////////////////////////////////////////////////////////////////////////////
/*  Use this function to set up expectations for the last called
    mocked function that has a return value.
    Syntax-wise this is usually used like this:
        ExpectCall(myFunction(0, 0.0, "hello)).Return(5); */

template <typename ReturnT>
inline FunctionOptions<ReturnT> ExpectCall(ReturnT /* dummyToDeduceReturnT */)
{
    boost::shared_ptr<Mock> lastCalledMock = MockRepository::GetLastCalledMock();
    RUDE_MOCKS_ASSERT(lastCalledMock);
    // if in release builds you crash here it most likely means that the mocked function got inlined
    // and therefore there is no last called mock
    return FunctionOptions<ReturnT>(lastCalledMock->GetMockRepository().GetLastExpectation());
}

/////////////////////////////////////////////////////////////////////////////
/*  Use this function to set up expectations for the last called mocked
    function that doesn't have a return value. */

inline FunctionOptions<void> LastCall()
{
    boost::shared_ptr<Mock> lastCalledMock = MockRepository::GetLastCalledMock();
    RUDE_MOCKS_ASSERT(lastCalledMock);
    // if in release builds you crash here it most likely means that the mocked function got inlined
    // and therefore there is no last called mock
    return FunctionOptions<void>(lastCalledMock->GetMockRepository().GetLastExpectation());
}

/////////////////////////////////////////////////////////////////////////////
/*  Use this function to stub a call. Stubbed functions can be called
    as many times as desired or not at all. Verification will not fail
    in either case. */

template <typename ReturnT>
inline FunctionOptions<ReturnT> StubCall(ReturnT /* dummyToDeduceReturnT */)
{
    boost::shared_ptr<Mock> lastCalledMock = MockRepository::GetLastCalledMock();
    RUDE_MOCKS_ASSERT(lastCalledMock);
    // if in release builds you crash here it most likely means that the mocked function got inlined
    // and therefore there is no last called mock
    boost::shared_ptr<ExpectationBase> lastExpectation = lastCalledMock->GetMockRepository().GetLastExpectation();
    lastCalledMock->GetMockRepository().AddStubbedCall(lastExpectation);
    return FunctionOptions<ReturnT>(lastExpectation);
}

/////////////////////////////////////////////////////////////////////////////
/*  Use this function to stub a call for the last called mocked
    function that doesn't have a return value. */

inline FunctionOptions<void> StubLastCall()
{
    boost::shared_ptr<Mock> lastCalledMock = MockRepository::GetLastCalledMock();
    RUDE_MOCKS_ASSERT(lastCalledMock);
    // if in release builds you crash here it most likely means that the mocked function got inlined
    // and therefore there is no last called mock
    boost::shared_ptr<ExpectationBase> lastExpectation = lastCalledMock->GetMockRepository().GetLastExpectation();
    lastCalledMock->GetMockRepository().AddStubbedCall(lastExpectation);
    return FunctionOptions<void>(lastExpectation);
}

}   // namespace RudeMocks

#endif  // RudeMocksMockFreeFunctions_INCLUDED
