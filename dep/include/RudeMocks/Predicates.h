/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksPredicates_INCLUDED
#define RudeMocksPredicates_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

// This file contains a number of binary predicates used as argument verifiers.

#include <RudeMocks/OperatorTraits.h>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/add_reference.hpp>
#include <boost/utility/enable_if.hpp>

namespace RudeMocks
{
namespace Predicate
{

/////////////////////////////////////////////////////////////////////////////

template <typename T>
struct PredicateBase
{
    typedef T ArgumentType;
    typedef typename boost::add_reference<typename boost::add_const<T>::type>::type ArgumentParamType;
};

template <typename T>
struct PredicateBase<T&>
{
    typedef T& ArgumentType;
    typedef typename boost::add_const<T>::type& ArgumentParamType;
};

template <typename T>
struct PredicateBase<T*>
{
    typedef T* ArgumentType;
    typedef typename boost::add_const<T>::type* ArgumentParamType;
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Ignore : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType /* expectedValue */, typename PredicateBase<T>::ArgumentParamType /* actualValue */) const
    {
        return true;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>     // use operator == if T provides one, else fall back to memcmp
struct IsEqual : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return memcmp(&expectedValue, &actualValue, sizeof(T)) == 0;
    }
};

template <typename T>
struct IsEqual<T, typename boost::enable_if<is_equality_comparable<T> >::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return expectedValue == actualValue;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>     // use operator != if T provides one, else fall back to operator == if provided, else fall back to memcmp
struct IsNotEqual : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return memcmp(&expectedValue, &actualValue, sizeof(T)) != 0;
    }
};

template <typename T>
struct IsNotEqual<T, typename boost::enable_if_c<is_inequality_comparable<T>::value && !is_equality_comparable<T>::value>::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return expectedValue != actualValue;
    }
};

template <typename T>
struct IsNotEqual<T, typename boost::enable_if_c<!is_inequality_comparable<T>::value && is_equality_comparable<T>::value>::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return !(expectedValue == actualValue);
    }
};

template <typename T>
struct IsNotEqual<T, typename boost::enable_if_c<is_inequality_comparable<T>::value && is_equality_comparable<T>::value>::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return expectedValue != actualValue;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>
struct IsLessThan : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return false;   // fail verification if operator < is not supported
    }
};

template <typename T>
struct IsLessThan<T, typename boost::enable_if<is_less_than_comparable<T> >::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return actualValue < expectedValue;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>
struct IsLessThanOrEqual : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return false;   // fail verification if operator <= is not supported
    }
};

template <typename T>
struct IsLessThanOrEqual<T, typename boost::enable_if<is_less_than_or_equal_comparable<T> >::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return actualValue <= expectedValue;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>
struct IsGreaterThan : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return false;   // fail verification if operator > is not supported
    }
};

template <typename T>
struct IsGreaterThan<T, typename boost::enable_if<is_greater_than_comparable<T> >::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return actualValue > expectedValue;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T, typename EnableIf = void>
struct IsGreaterThanOrEqual : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return false;   // fail verification if operator >= is not supported
    }
};

template <typename T>
struct IsGreaterThanOrEqual<T, typename boost::enable_if<is_greater_than_or_equal_comparable<T> >::type> : public PredicateBase<T>
{
    bool operator () (typename PredicateBase<T>::ArgumentParamType expectedValue, typename PredicateBase<T>::ArgumentParamType actualValue) const
    {
        return actualValue >= expectedValue;
    }
};

}   // namespace Predicate
}   // namespace RudeMocks

#endif  // RudeMocksPredicates_INCLUDED


