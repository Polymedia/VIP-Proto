/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

// This code is based on Boost's is_incrementable.hpp.

#ifndef RudeMocksIsEqualityComparable_INCLUDED
#define RudeMocksIsEqualityComparable_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/empty.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <iostream>
 
namespace RudeMocks
{
namespace Detail
{

/////////////////////////////////////////////////////////////////////////////
// A tag type returned by the operators for the any struct in this namespace
// when T does not support the operator.

struct tag {};

/////////////////////////////////////////////////////////////////////////////
// This type soaks up any implicit conversions and makes the following operators
// less preferred than any other such operators found via ADL.

struct any
{
    // Conversion constructor for any type.
    template <typename T>
    any(T const&);
};

/////////////////////////////////////////////////////////////////////////////
// Fallback operators for types that don't support them.

tag operator == (any const&, any const&);
tag operator != (any const&, any const&);
tag operator < (any const&, any const&);
tag operator <= (any const&, any const&);
tag operator > (any const&, any const&);
tag operator >= (any const&, any const&);
tag operator << (std::ostream&, any const&);

/////////////////////////////////////////////////////////////////////////////
/*  Two overloads to distinguish whether T supports a certain operator 
    expression. The first overload returns a reference to a two-element 
    character array and is chosen if T does not support the expression, 
    such as ==, whereas the second overload returns a char directly and is 
    chosen if T supports the expression. So using sizeof(check(<expression>))
    returns 2 for the first overload and 1 for the second overload. */

typedef char yes_type;
typedef char (&no_type)[2];

no_type check(tag);

template <typename T>
yes_type check(T const&);

/////////////////////////////////////////////////////////////////////////////
// Base class for all our operator trait implementation classes.

template <typename T>
struct operator_traits_impl_base
{
    static typename boost::remove_cv<typename boost::remove_reference<T>::type>::type const& x;
};

/////////////////////////////////////////////////////////////////////////////
/*  Define the actual operator traits template classes using preprocessor 
    metaprogramming. The expanded classes look like this:

    template <typename T>
    struct is_equality_comparable_impl : public operator_traits_impl_base<T>
    {
        using operator_traits_impl_base<T>::x;
        static const bool value = sizeof(check(x == x)) == sizeof(yes_type);
    };

    template <typename T>
    struct is_equality_comparable : Detail::is_equality_comparable_impl<T> {}; */

#define OPERATOR_TRAIT_NAME_PAIRS \
    ((==, is_equality_comparable)) \
    ((!=, is_inequality_comparable)) \
    ((<, is_less_than_comparable)) \
    ((<=, is_less_than_or_equal_comparable)) \
    ((>, is_greater_than_comparable)) \
    ((>=, is_greater_than_or_equal_comparable))

#define DEFINE_OPERATOR_TRAIT_IMPL_CLASS(r, data, elem) \
    template <typename T> \
    struct BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 1, elem), _impl) : public operator_traits_impl_base<T> \
    { \
        using operator_traits_impl_base<T>::x; \
        static const bool value = sizeof(check(x BOOST_PP_TUPLE_ELEM(2, 0, elem) x)) == sizeof(yes_type); \
    };

BOOST_PP_SEQ_FOR_EACH(DEFINE_OPERATOR_TRAIT_IMPL_CLASS, BOOST_PP_EMPTY(), OPERATOR_TRAIT_NAME_PAIRS)

template <class T>
struct is_output_streamable_impl : public operator_traits_impl_base<T>
{
    using operator_traits_impl_base<T>::x;
    static const bool value = sizeof(check(std::cout << x)) == sizeof(yes_type);
};
 
}   // namespace Detail

#define DEFINE_OPERATOR_TRAIT_CLASS(r, data, elem) \
    template <typename T> \
    struct BOOST_PP_TUPLE_ELEM(2, 1, elem) : Detail::BOOST_PP_CAT(BOOST_PP_TUPLE_ELEM(2, 1, elem), _impl)<T> {};

BOOST_PP_SEQ_FOR_EACH(DEFINE_OPERATOR_TRAIT_CLASS, BOOST_PP_EMPTY(), OPERATOR_TRAIT_NAME_PAIRS)

#undef DEFINE_OPERATOR_TRAIT_CLASS
#undef DEFINE_OPERATOR_TRAIT_IMPL_CLASS
#undef OPERATOR_TRAIT_NAME_PAIRS

template <class T>
struct is_output_streamable : Detail::is_output_streamable_impl<T> {};

}   // namespace RudeMocks

#endif  // RudeMocksIsEqualityComparable_INCLUDED
