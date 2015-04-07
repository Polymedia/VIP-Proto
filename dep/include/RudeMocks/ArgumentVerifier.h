/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksArgumentVerifier_INCLUDED
#define RudeMocksArgumentVerifier_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/ValuePrinter.h>
#include <RudeMocks/ArgumentValue.h>
#include <boost/any.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////

class ArgumentVerifierBase
{
public:
    virtual ~ArgumentVerifierBase() {}
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class ArgumentVerifier : public ArgumentVerifierBase
{
public:
    typedef ArgumentT ArgumentType;

    virtual bool Verify(std::size_t argumentIndex, ArgumentValue<ArgumentT> expectedValue, ArgumentValue<ArgumentT> actualValue, std::string& description) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT, typename PredicateT>
class ArgumentVerifierPredicate : public ArgumentVerifier<ArgumentT>
{
public:
    explicit ArgumentVerifierPredicate(PredicateT predicate, const boost::any& expectedValue = boost::any())
        :   m_predicate(predicate), m_expectedValue(expectedValue)
    {}

    virtual bool Verify(std::size_t argumentIndex, ArgumentValue<ArgumentT> expectedValue, ArgumentValue<ArgumentT> actualValue, std::string& description) const
    {
        bool succeeded = m_predicate(HasStoredExpectedValue() ? GetStoredExpectedValue() : expectedValue.GetValue(), actualValue.GetValue());

        if (!succeeded)
        {
            ValuePrinter<ArgumentT> printer;
            std::ostringstream stream;
            stream << "Argument verification failed for argument " << argumentIndex << " with predicate = " << typeid(PredicateT).name() 
                << ", expected = " << printer(expectedValue.GetValue()) << ", actual = " << printer(actualValue.GetValue());
            description = stream.str();
        }

        return succeeded;
    }

private:
    bool HasStoredExpectedValue() const
    {
        return !m_expectedValue.empty();
    }

    ArgumentT GetStoredExpectedValue() const
    {
        return boost::any_cast<ArgumentT>(m_expectedValue);
    }

private:
    PredicateT m_predicate;
    mutable boost::any m_expectedValue;     // mutable ensures that non-const reference values stored in the any work
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT, typename PredicateT>
boost::shared_ptr<ArgumentVerifierBase> MakeArgumentVerifierPredicate(PredicateT predicate, const boost::any& expectedValue = boost::any())
{
    return boost::shared_ptr<ArgumentVerifierBase>(new ArgumentVerifierPredicate<ArgumentT, PredicateT>(predicate, expectedValue));
}

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class CustomArgumentVerifier : public ArgumentVerifier<ArgumentT>
{
public:
    typedef boost::function<bool (std::size_t argumentIndex, ArgumentValue<ArgumentT> expectedValue,
        ArgumentValue<ArgumentT> actualValue, std::string& description)> Function;

    explicit CustomArgumentVerifier(const Function& function)
        :   m_function(function)
    {
        RUDE_MOCKS_ASSERT(!function.empty());
    }

    virtual bool Verify(std::size_t argumentIndex, ArgumentValue<ArgumentT> expectedValue, ArgumentValue<ArgumentT> actualValue, std::string& description) const
    {
        return m_function(argumentIndex, expectedValue, actualValue, description);
    }

private:
    Function m_function;
};

}   // namespace RudeMocks

#endif  // RudeMocksArgumentVerifier_INCLUDED
