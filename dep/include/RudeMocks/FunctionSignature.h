/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksFunctionSignature_INCLUDED
#define RudeMocksFunctionSignature_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/fusion/container/vector.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A template traits class that stores various information about a given
    function pointer, such as its arity, the return type, all argument
    types, etc. */

template <typename FunctionPtrT>
struct FunctionSignature;

#define RUDE_MOCKS_ARGUMENT_TYPEDEF(z, n, _) \
    typedef BOOST_PP_CAT(T, n) BOOST_PP_CAT(ArgumentType, n);

/////////////////////////////////////////////////////////////////////////////
/*  Function signature implementations for member function pointers up to
    a certain user-configurable arity.
    For example, for a member function with type ReturnT (ObjectT::*)(T0)
    the following would expand to:

    template <typename ReturnT, typename ObjectT, typename T0>
    struct FunctionSignature<ReturnT (ObjectT::*)(T0)>
    {
        typedef ObjectT ObjectType;
        typedef ReturnT (ObjectT::*FunctionPtrType)(T0);
        typedef ReturnT ReturnType;
        typedef T0 ArgumentType0;
        typedef boost::fusion::vector<T0> ArgumentVectorType;
        static const std::size_t arity = 1;
    };
*/

#define RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE(z, n, constQualifierAndcallingConventionTuple) \
template <typename ReturnT, typename ObjectT BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)> \
struct FunctionSignature<ReturnT (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple)> \
{ \
    typedef ObjectT ObjectType; \
    typedef ReturnT (BOOST_PP_TUPLE_ELEM(2, 1, constQualifierAndcallingConventionTuple) ObjectT::*FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)) BOOST_PP_TUPLE_ELEM(2, 0, constQualifierAndcallingConventionTuple); \
    typedef ReturnT ReturnType; \
    BOOST_PP_REPEAT(n, RUDE_MOCKS_ARGUMENT_TYPEDEF, BOOST_PP_EMPTY()) \
    typedef boost::fusion::vector<BOOST_PP_ENUM_PARAMS(n, T)> ArgumentVectorType; \
    static const std::size_t arity = n; \
};

BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (BOOST_PP_EMPTY(), BOOST_PP_EMPTY()))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (const, BOOST_PP_EMPTY()))
#ifdef RUDE_MOCKS_SUPPORTS_MEMBER_FUNCTION_CALLING_CONVENTIONS
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (BOOST_PP_EMPTY(), __cdecl))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (const, __cdecl))
#ifdef RUDE_MOCKS_HAS_STDCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (BOOST_PP_EMPTY(), __stdcall))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (const, __stdcall))
#endif
#ifdef RUDE_MOCKS_HAS_FASTCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (BOOST_PP_EMPTY(), __fastcall))
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE, (const, __fastcall))
#endif
#endif  // RUDE_MOCKS_SUPPORTS_MEMBER_FUNCTION_CALLING_CONVENTIONS

#undef RUDE_MOCKS_DEFINE_MEMBER_FUNCTION_SIGNATURE

/////////////////////////////////////////////////////////////////////////////
/*  Function signature implementations for free function pointers up to
    a certain user-configurable arity.
    For example, for a member function with type ReturnT (*)(T0)
    the following would expand to:

    template <typename ReturnT, typename T0>
    struct FunctionSignature<ReturnT (*)(T0)>
    {
        typedef ReturnT (*FunctionPtrType)(T0);
        typedef ReturnT ReturnType;
        typedef T0 ArgumentType0;
        typedef boost::fusion::vector<T0> ArgumentVectorType;
        static const std::size_t arity = 1;
    };
*/

#define RUDE_MOCKS_DEFINE_FREE_FUNCTION_SIGNATURE(z, n, callingConvention) \
template <typename ReturnT BOOST_PP_ENUM_TRAILING_PARAMS(n, typename T)> \
struct FunctionSignature<ReturnT (callingConvention *)(BOOST_PP_ENUM_PARAMS(n, T))> \
{ \
    typedef ReturnT (callingConvention *FunctionPtrType)(BOOST_PP_ENUM_PARAMS(n, T)); \
    typedef ReturnT ReturnType; \
    BOOST_PP_REPEAT(n, RUDE_MOCKS_ARGUMENT_TYPEDEF, BOOST_PP_EMPTY()) \
    typedef boost::fusion::vector<BOOST_PP_ENUM_PARAMS(n, T)> ArgumentVectorType; \
    static const std::size_t arity = n; \
};

BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_SIGNATURE, __cdecl)
#ifdef RUDE_MOCKS_HAS_STDCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_SIGNATURE, __stdcall)
#endif
#ifdef RUDE_MOCKS_HAS_FASTCALL
BOOST_PP_REPEAT(BOOST_PP_INC(RUDE_MOCKS_MAX_FUNCTION_ARITY), RUDE_MOCKS_DEFINE_FREE_FUNCTION_SIGNATURE, __fastcall)
#endif

#undef RUDE_MOCKS_DEFINE_FREE_FUNCTION_SIGNATURE

#undef RUDE_MOCKS_ARGUMENT_TYPEDEF

}   // namespace RudeMocks

#endif  // RudeMocksFunctionSignature_INCLUDED
