/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksValuePrinter_INCLUDED
#define RudeMocksValuePrinter_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/OperatorTraits.h>
#include <boost/type_traits/add_reference.hpp>
#include <boost/type_traits/add_const.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/utility/enable_if.hpp>
#include <typeinfo>
#include <string>
#include <sstream>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A function object that prints a given value of type T. */

template <typename T, typename EnableIf = void>
class ValuePrinter
{
public:
    typedef typename boost::add_reference<typename boost::add_const<T>::type>::type ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << typeid(T).name() << " @ 0x" << std::hex << reinterpret_cast<std::size_t>(&value);
        return stream.str();
    }
};

/////////////////////////////////////////////////////////////////////////////
/*  Specialization for pointer types. */

template <typename T>
class ValuePrinter<T*, typename boost::enable_if_c<!boost::is_void<T>::value>::type>
{
public:
    typedef typename boost::add_const<T>::type* ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << typeid(T*).name() << " -> 0x" << std::hex << reinterpret_cast<std::size_t>(value)
            << std::dec << ": " << ValuePrinter<T>()(*value);
        return stream.str();
    }
};

template <typename T>
class ValuePrinter<T*, typename boost::enable_if<boost::is_void<T> >::type>
{
public:
    typedef typename boost::add_const<T>::type* ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << typeid(T*).name() << " -> 0x" << std::hex << reinterpret_cast<std::size_t>(value) << std::dec;
        return stream.str();
    }
};

/////////////////////////////////////////////////////////////////////////////
/*  Specialization that is used when T supports the output stream
    operator <<. */

template <typename T>
class ValuePrinter<T, typename boost::enable_if_c<is_output_streamable<T>::value && !boost::is_pointer<T>::value && !boost::is_reference<T>::value>::type>
{
public:
    typedef typename boost::add_reference<typename boost::add_const<T>::type>::type ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class ValuePrinter<T&, typename boost::enable_if<is_output_streamable<T> >::type>
{
public:
    typedef typename boost::add_const<T>::type& ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << value;
        return stream.str();
    }
};

/////////////////////////////////////////////////////////////////////////////

template <typename T>
class ValuePrinter<T&, typename boost::disable_if<is_output_streamable<T> >::type>
{
public:
    typedef typename boost::add_const<T>::type& ParamType;

    std::string operator () (ParamType value) const
    {
        std::ostringstream stream;
        stream << typeid(T).name() << " @ 0x" << std::hex << reinterpret_cast<std::size_t>(&value);
        return stream.str();
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksValuePrinter_INCLUDED

