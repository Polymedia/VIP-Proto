/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksFunctionRecorderAndReplayer_INCLUDED
#define RudeMocksFunctionRecorderAndReplayer_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/FunctionSignature.h>
#include <RudeMocks/FunctionArguments.h>
#include <RudeMocks/GetDefaultValue.h>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

namespace RudeMocks
{

class IMockRecordAndReplay;

/////////////////////////////////////////////////////////////////////////////
/*  The base class for all function recorders and replayers. Derived
    classes for specific function pointer types are generated using
    preprocessor metaprogramming. These classes are used directly by
    the mock function thunks and offer Record and Replay functions that
    exactly match the signature of the mocked function. They merely package
    the passed in arguments into a Boost.Fusion vector and dispatch the
    call to the actual mock object. */

class FunctionRecorderAndReplayerBase
{
public:
    explicit FunctionRecorderAndReplayerBase(const boost::shared_ptr<IMockRecordAndReplay>& mock)
        :   m_mock(mock)
    {
        RUDE_MOCKS_ASSERT(mock);
    }
    virtual ~FunctionRecorderAndReplayerBase();

    boost::shared_ptr<IMockRecordAndReplay> GetMock() const { return boost::shared_ptr<IMockRecordAndReplay>(m_mock); }

    void MakeCurrent() { sm_current = this; }
    static FunctionRecorderAndReplayerBase* GetCurrent() { RUDE_MOCKS_ASSERT(sm_current); return sm_current; }

    void RecordToMock(const boost::shared_ptr<const FunctionArgumentsBase> functionArguments) const;
    boost::any ReplayFromMock(const boost::shared_ptr<FunctionArgumentsBase> functionArguments) const;

private:
    static FunctionRecorderAndReplayerBase* sm_current;
    boost::weak_ptr<IMockRecordAndReplay> m_mock;
};

/////////////////////////////////////////////////////////////////////////////
/*  Intentionally left undefined. If you get a compile-time error in
    the following line it probably means that the specific function
    you're trying to mock is not yet supported (for example, because of
    an unsupported calling convention). */

template <typename FunctionPtrT, typename EnableIf = void>
struct FunctionRecorderAndReplayer;

/////////////////////////////////////////////////////////////////////////////
/*  Define member function recorder and replayer classes for the maximum
    supported function arity using preprocessor metaprogramming. */

#define RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER(z, n, constQualifierAndcallingConventionTuple) \
template <typename ReturnT, typename ObjectT BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)> \
struct FunctionRecorderAndReplayer<ReturnT (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple), typename boost::disable_if<boost::is_same<ReturnT, void> >::type> : public FunctionRecorderAndReplayerBase \
{ \
    typedef ReturnT (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple); \
    typedef typename FunctionSignature<FunctionPtrType>::ArgumentVectorType ArgumentVectorType; \
    typedef typename boost::remove_cv<typename boost::remove_reference<ReturnT>::type>::type NonRefNonCVReturnType; \
\
    explicit FunctionRecorderAndReplayer(const boost::shared_ptr<IMockRecordAndReplay>& mock) \
        :   FunctionRecorderAndReplayerBase(mock), m_defaultReturnValue(GetDefaultValue<NonRefNonCVReturnType>()()) \
    {} \
\
    ReturnT BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) Record(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        RecordToMock(functionArguments); \
        return m_defaultReturnValue; \
    } \
\
    ReturnT BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) Replay(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        boost::any returnValue = ReplayFromMock(functionArguments); \
        if (returnValue.empty()) \
            return m_defaultReturnValue; \
        else \
        { \
            return boost::is_reference<ReturnT>::value ? \
                boost::any_cast<boost::reference_wrapper<typename boost::remove_reference<ReturnT>::type > >(returnValue).get() : \
                boost::any_cast<ReturnT>(returnValue); \
        } \
    } \
\
private: \
    mutable NonRefNonCVReturnType m_defaultReturnValue; /* needs to be mutable for const member functions in case we have to return non-const references from Record or Replay. */ \
}; \
\
template <typename ObjectT BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)> \
struct FunctionRecorderAndReplayer<void (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple)> : public FunctionRecorderAndReplayerBase \
{ \
    typedef void (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple); \
    typedef typename FunctionSignature<FunctionPtrType>::ArgumentVectorType ArgumentVectorType; \
\
    explicit FunctionRecorderAndReplayer(const boost::shared_ptr<IMockRecordAndReplay>& mock) \
        :   FunctionRecorderAndReplayerBase(mock) \
    {} \
\
    void BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) Record(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        RecordToMock(functionArguments); \
    } \
\
    void BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) Replay(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        ReplayFromMock(functionArguments); \
    } \
};

BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (BOOST_PP_EMPTY(), BOOST_PP_EMPTY()))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (const, BOOST_PP_EMPTY()))
#ifdef RUDE_MOCKS_SUPPORTS_MEMBER_FUNCTION_CALLING_CONVENTIONS
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (BOOST_PP_EMPTY(), __cdecl))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (const, __cdecl))
#ifdef RUDE_MOCKS_HAS_STDCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (BOOST_PP_EMPTY(), __stdcall))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (const, __stdcall))
#endif
#ifdef RUDE_MOCKS_HAS_FASTCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (BOOST_PP_EMPTY(), __fastcall))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER, (const, __fastcall))
#endif
#endif  // RUDE_MOCKS_SUPPORTS_MEMBER_FUNCTION_CALLING_CONVENTIONS

#undef RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_RECORDER_AND_REPLAYER

/////////////////////////////////////////////////////////////////////////////
/*  Define free function recorder and replayer classes for the maximum
    supported function arity using preprocessor metaprogramming. */

#define RUDE_MOCKS_DEFINE_FREE_FUNCTION_RECORDER_AND_REPLAYER(z, n, callingConvention) \
template <typename ReturnT BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)> \
struct FunctionRecorderAndReplayer<ReturnT (callingConvention *)(BOOST_PP_ENUM_PARAMS(n, T)), typename boost::disable_if<boost::is_same<ReturnT, void> >::type> : public FunctionRecorderAndReplayerBase \
{ \
    typedef ReturnT (callingConvention *FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)); \
    typedef FunctionRecorderAndReplayer<FunctionPtrType> ThisType; \
    typedef typename FunctionSignature<FunctionPtrType>::ArgumentVectorType ArgumentVectorType; \
    typedef typename boost::remove_cv<typename boost::remove_reference<ReturnT>::type>::type NonRefNonCVReturnType; \
\
    explicit FunctionRecorderAndReplayer(const boost::shared_ptr<IMockRecordAndReplay>& mock) \
        :   FunctionRecorderAndReplayerBase(mock), m_defaultReturnValue(GetDefaultValue<NonRefNonCVReturnType>()()) \
    {} \
\
    static ReturnT callingConvention Record(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        GetCurrent()->RecordToMock(functionArguments); \
        return static_cast<ThisType*>(GetCurrent())->m_defaultReturnValue; \
    } \
\
    static ReturnT callingConvention Replay(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        boost::any returnValue = GetCurrent()->ReplayFromMock(functionArguments); \
        if (returnValue.empty()) \
            return static_cast<ThisType*>(GetCurrent())->m_defaultReturnValue; \
        else \
        { \
            return boost::is_reference<ReturnT>::value ? \
                boost::any_cast<boost::reference_wrapper<typename boost::remove_reference<ReturnT>::type > >(returnValue).get() : \
                boost::any_cast<ReturnT>(returnValue); \
        } \
    } \
\
private: \
    NonRefNonCVReturnType m_defaultReturnValue; \
}; \
\
template <BOOST_PP_ENUM_PARAMS(n, typename T)> /* note that if this expands to nothing when n == 0 we mustn't use typename on dependent types */ \
struct FunctionRecorderAndReplayer<void (callingConvention *)(BOOST_PP_ENUM_PARAMS(n, T))> : public FunctionRecorderAndReplayerBase \
{ \
    typedef void (callingConvention *FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)); \
    typedef BOOST_PP_IF(n, typename, BOOST_PP_EMPTY()) FunctionSignature<FunctionPtrType>::ArgumentVectorType ArgumentVectorType; \
\
    explicit FunctionRecorderAndReplayer(const boost::shared_ptr<IMockRecordAndReplay>& mock) \
        :   FunctionRecorderAndReplayerBase(mock) \
    {} \
\
    static void callingConvention Record(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        GetCurrent()->RecordToMock(functionArguments); \
    } \
\
    static void callingConvention Replay(BOOST_PP_ENUM_BINARY_PARAMS(n, T, t)) \
    { \
        boost::shared_ptr<FunctionArgumentsBase> functionArguments(new FunctionArguments<ArgumentVectorType>(ArgumentVectorType(BOOST_PP_ENUM_PARAMS(n, t)))); \
        GetCurrent()->ReplayFromMock(functionArguments); \
    } \
};

BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_RECORDER_AND_REPLAYER, __cdecl)
#ifdef RUDE_MOCKS_HAS_STDCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_RECORDER_AND_REPLAYER, __stdcall)
#endif
#ifdef RUDE_MOCKS_HAS_FASTCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_RECORDER_AND_REPLAYER, __fastcall)
#endif

#undef RUDE_MOCKS_DEFINE_FREE_FUNCTION_RECORDER_AND_REPLAYER

}   // namespace RudeMocks

#endif  // RudeMocksFunctionRecorderAndReplayer_INCLUDED
