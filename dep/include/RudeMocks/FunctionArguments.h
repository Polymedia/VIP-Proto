/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksFunctionArguments_INCLUDED
#define RudeMocksFunctionArguments_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/FunctionArguments.h>
#include <RudeMocks/ValuePrinter.h>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/fusion/algorithm/transformation/transform.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A template metafunction that given an argument type returns the type
    that is to be stored in a FunctionArguments object. Additionally,
    it provides a static member function called Store that is called
    at runtime whenever a function call is recorded or replayed to convert
    from the actual argument type to the stored argument type. */

template <typename ArgumentT>
struct ArgumentStorage
{
private:
    template <typename T>
    struct RemoveReferenceIfIsConstReference
    {
        typedef typename boost::remove_reference<T>::type NoRefType;
        typedef typename boost::mpl::if_<boost::is_const<NoRefType>, typename boost::remove_const<NoRefType>::type, T>::type type;
    };

public:
    typedef ArgumentT ArgumentType;
    typedef typename RemoveReferenceIfIsConstReference<ArgumentType>::type StoredArgumentType;

    explicit ArgumentStorage(ArgumentType value)
        :   m_storedValue(value)
    {}

    ArgumentType Get() const
    {
        return m_storedValue;
    }

private:
    StoredArgumentType m_storedValue;
};

/////////////////////////////////////////////////////////////////////////////
/*  The base function argument container class. Deriving classes store
    a Boost.Fusion vector of argument values. */

class FunctionArgumentsBase
{
public:
    virtual std::string GetAsString() const = 0;
    virtual std::size_t GetArgumentCount() const = 0;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentVectorT>
class FunctionArguments : public FunctionArgumentsBase
{
private:
    template <typename SequenceT>
    struct GetStoredArgumentVectorType : public boost::mpl::transform<SequenceT, boost::mpl::identity<ArgumentStorage<boost::mpl::_> > >
    {};

public:
    typedef ArgumentVectorT ArgumentVectorType;
    typedef typename GetStoredArgumentVectorType<ArgumentVectorType>::type StoredArgumentVectorType;
    typedef boost::mpl::range_c<int, 0, boost::mpl::size<ArgumentVectorType>::value> ArgumentIntegralConstantRange;

private:
    struct StoreArgument
    {
        template <typename SignatureT>
        struct result;

        template <typename SelfT, typename T, typename IntT>
        struct result<SelfT(T, IntT)>
        {
            typedef ArgumentStorage<typename boost::mpl::at<ArgumentVectorType, IntT>::type> type;
        };

        template <typename T, typename IntT>
        ArgumentStorage<typename boost::mpl::at<ArgumentVectorType, IntT>::type> operator () (T& value, IntT) const     // needs to be a reference, so that reference argument types will work correctly
        {
            return ArgumentStorage<typename boost::mpl::at<ArgumentVectorType, IntT>::type>(value);
        }
    };

    struct PrintValue
    {
        explicit PrintValue(const FunctionArguments<ArgumentVectorT>& functionArguments, std::string& valuesAsString)
            :   m_functionArguments(functionArguments), m_valuesAsString(valuesAsString)
        {}

       template <typename IntT>
        void operator () (IntT) const
        {
            typedef typename boost::mpl::at<ArgumentVectorType, IntT>::type ArgumentType;

            ValuePrinter<ArgumentType> printer;
            m_valuesAsString += printer(m_functionArguments.GetArgument<IntT::value>());
            m_valuesAsString += ',';
        }

    private:
        const FunctionArguments<ArgumentVectorT>& m_functionArguments;
        std::string& m_valuesAsString;
    };

public:
    explicit FunctionArguments(const ArgumentVectorType& arguments)
        :   m_arguments(boost::fusion::transform(arguments, ArgumentIntegralConstantRange(), StoreArgument()))
    {}

    virtual std::string GetAsString() const
    {
        std::string valuesAsString("args=[");

        boost::mpl::for_each<ArgumentIntegralConstantRange>(PrintValue(*this, valuesAsString));

        if (valuesAsString[valuesAsString.length() - 1] == ',')
            valuesAsString = valuesAsString.substr(0, valuesAsString.length() - 1);
        valuesAsString += ']';
        return valuesAsString;
    }

    virtual std::size_t GetArgumentCount() const { return boost::fusion::size(m_arguments); }
    const StoredArgumentVectorType& GetStoredArguments() const { return m_arguments; }
    StoredArgumentVectorType& GetStoredArguments() { return m_arguments; }

    template <int N>
    typename boost::mpl::at_c<ArgumentVectorType, N>::type GetArgument() const
    {
        return boost::fusion::at_c<N>(GetStoredArguments()).Get();
    }

private:
    StoredArgumentVectorType m_arguments;
};

}   // namespace RudeMocks

#endif  // RudeMocksFunctionArguments_INCLUDED
