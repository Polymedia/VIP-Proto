/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksArgumentOptions_INCLUDED
#define RudeMocksArgumentOptions_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/ArgumentVerifier.h>
#include <RudeMocks/Predicates.h>
#include <RudeMocks/IsOutArgumentType.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/type_traits/is_reference.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_pointer.hpp>

namespace RudeMocks
{

class ExpectationBase;
class FunctionOptionsBase;

/////////////////////////////////////////////////////////////////////////////
/*  The abstract base class for all argument options. These are stored by
    an expectation, one for each argument, and returned from
    FunctionOptions::Arg. Deriving classes are for specific argument types. */

class ArgumentOptionsBase : private boost::noncopyable
{
public:
    ArgumentOptionsBase(const boost::shared_ptr<ExpectationBase>& expectation, std::size_t argumentIndex);
    virtual ~ArgumentOptionsBase();

    boost::shared_ptr<ExpectationBase> GetExpectation() const { return boost::shared_ptr<ExpectationBase>(m_expectation); }
    std::size_t GetArgumentIndex() const { return m_argumentIndex; }

    virtual const FunctionOptionsBase& Ignore() const = 0;
    virtual const FunctionOptionsBase& IsEqual(const boost::any& expectedValue = boost::any()) const = 0;
    virtual const FunctionOptionsBase& IsNotEqual(const boost::any& expectedValue = boost::any()) const = 0;

    virtual const FunctionOptionsBase& IsLessThan(const boost::any& expectedValue = boost::any()) const = 0;
    virtual const FunctionOptionsBase& IsLessThanOrEqual(const boost::any& expectedValue = boost::any()) const = 0;
    virtual const FunctionOptionsBase& IsGreaterThan(const boost::any& expectedValue = boost::any()) const = 0;
    virtual const FunctionOptionsBase& IsGreaterThanOrEqual(const boost::any& expectedValue = boost::any()) const = 0;

    template <typename FunctionT>
    const FunctionOptionsBase& Do(const FunctionT& function) const
    {
        return SetCustomArgumentVerifier(boost::any(function));
    }

protected:
    const FunctionOptionsBase& SetArgumentVerifier(const boost::shared_ptr<ArgumentVerifierBase>& argumentVerifier) const;
    virtual const FunctionOptionsBase& SetCustomArgumentVerifier(const boost::any& function) const = 0;

    const FunctionOptionsBase& GetCurrentFunctionOptions() const { RUDE_MOCKS_ASSERT(m_currentFunctionOptions); return *m_currentFunctionOptions; }

    template <typename T>
    bool CanCastAnyTo(boost::any value) const
    {
        try
        {
            boost::any_cast<T>(value);
            return true;
        }
        catch (const boost::bad_any_cast&)
        {
        	return false;
        }
    }

private:
    friend class FunctionOptionsBase;
    void SetCurrentFunctionOptions(const FunctionOptionsBase* functionOptions);

private:
    boost::weak_ptr<ExpectationBase> m_expectation;
    std::size_t m_argumentIndex;
    const FunctionOptionsBase* m_currentFunctionOptions;
};

/////////////////////////////////////////////////////////////////////////////

class OutArgumentOptionsBase : public ArgumentOptionsBase
{
public:
    OutArgumentOptionsBase(const boost::shared_ptr<ExpectationBase>& expectation, std::size_t argumentIndex)
        :   ArgumentOptionsBase(expectation, argumentIndex)
    {}

    virtual const FunctionOptionsBase& Return(const boost::any& outValue) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class OutArgumentOptions : public OutArgumentOptionsBase
{
    RUDE_MOCKS_STATIC_ASSERT(IsOutArgumentType<ArgumentT>::value);

    typedef typename boost::mpl::eval_if
    <
        boost::is_reference<ArgumentT>,
        boost::remove_reference<ArgumentT>,
        boost::remove_pointer<ArgumentT>
    >::type OutArgumentBaseType;

public:
    OutArgumentOptions(const boost::shared_ptr<ExpectationBase>& expectation, std::size_t argumentIndex)
        :   OutArgumentOptionsBase(expectation, argumentIndex)
    {}

    virtual const FunctionOptionsBase& Return(const boost::any& outValue) const
    {
        RUDE_MOCKS_ASSERT_MSG(CanCastAnyTo<OutArgumentBaseType>(outValue), "The given out value is not of the correct type. For T& or T* it needs to be T.");
        GetExpectation()->SetOutArgumentValue(GetArgumentIndex(), outValue);
        return GetCurrentFunctionOptions();
    }
};

template <>
class OutArgumentOptions<void*> : public OutArgumentOptionsBase
{
public:
    OutArgumentOptions(const boost::shared_ptr<ExpectationBase>& expectation, std::size_t argumentIndex)
        :   OutArgumentOptionsBase(expectation, argumentIndex)
    {}

    virtual const FunctionOptionsBase& Return(const boost::any& /*outValue*/) const
    {
        RUDE_MOCKS_ASSERT_MSG(false, "Calling Return() is currently unsupported on void* out argument types.");
        return GetCurrentFunctionOptions();
    }
};

/////////////////////////////////////////////////////////////////////////////

namespace Detail
{

template <typename ArgumentT>
struct GetArgumentOptionsBase : public boost::mpl::if_<IsOutArgumentType<ArgumentT>, OutArgumentOptions<ArgumentT>, ArgumentOptionsBase>
{};

}   // namespace Detail

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class ArgumentOptions : public Detail::GetArgumentOptionsBase<ArgumentT>::type
{
private:
    typedef typename Detail::GetArgumentOptionsBase<ArgumentT>::type BaseType;

public:
    ArgumentOptions(const boost::shared_ptr<ExpectationBase>& expectation, std::size_t argumentIndex)
        :   BaseType(expectation, argumentIndex)
    {}

    virtual const FunctionOptionsBase& Ignore() const
    {
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::Ignore<ArgumentT>()));
    }
    virtual const FunctionOptionsBase& IsEqual(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsEqual<ArgumentT>(), expectedValue));
    }   
    virtual const FunctionOptionsBase& IsNotEqual(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsNotEqual<ArgumentT>(), expectedValue));
    }
    virtual const FunctionOptionsBase& IsLessThan(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsLessThan<ArgumentT>(), expectedValue));
    }
    virtual const FunctionOptionsBase& IsLessThanOrEqual(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsLessThanOrEqual<ArgumentT>(), expectedValue));
    }
    virtual const FunctionOptionsBase& IsGreaterThan(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsGreaterThan<ArgumentT>(), expectedValue));
    }
    virtual const FunctionOptionsBase& IsGreaterThanOrEqual(const boost::any& expectedValue = boost::any()) const
    {
        RUDE_MOCKS_ASSERT_MSG(expectedValue.empty() || BaseType::template CanCastAnyTo<ArgumentT>(expectedValue), "The given expected value is not of the correct type. It needs to be of type ArgumentT.");
        return SetArgumentVerifier(MakeArgumentVerifierPredicate<ArgumentT>(Predicate::IsGreaterThanOrEqual<ArgumentT>(), expectedValue));
    }

private:
    virtual const FunctionOptionsBase& SetCustomArgumentVerifier(const boost::any& function) const
    {
        typedef CustomArgumentVerifier<ArgumentT> CustomArgumentVerifierType;
        typedef typename CustomArgumentVerifierType::Function Function;
        
        const Function* verifierFunction = boost::any_cast<Function>(&function);
        RUDE_MOCKS_ASSERT_MSG(verifierFunction, "Custom argument verifier function doesn't have the correct type. See CustomArgumentVerifier::Function.");

        boost::shared_ptr<CustomArgumentVerifierType> argumentVerifier(new CustomArgumentVerifierType(*verifierFunction));
        return SetArgumentVerifier(argumentVerifier);
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksArgumentOptions_INCLUDED

