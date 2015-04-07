/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksFunctionOptions_INCLUDED
#define RudeMocksFunctionOptions_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/shared_ptr.hpp>
#include <RudeMocks/Expectation.h>

namespace RudeMocks
{

class ArgumentOptionsBase;

/////////////////////////////////////////////////////////////////////////////

class FunctionOptionsBase
{
public:
    explicit FunctionOptionsBase(const boost::shared_ptr<ExpectationBase>& expectation);
    virtual ~FunctionOptionsBase();

    const boost::shared_ptr<ExpectationBase>& GetExpectation() const { return m_expectation; }
    const OutArgumentOptionsBase& OutArg(std::size_t argumentIndex) const;
    const OutArgumentOptionsBase& InOutArg(std::size_t argumentIndex) const;

    const ArgumentOptionsBase& Arg(std::size_t argumentIndex) const;
    const FunctionOptionsBase& IgnoreArguments() const;
    const FunctionOptionsBase& Repeat(std::size_t minCallCount, std::size_t maxCallCount) const;
    const FunctionOptionsBase& Repeat(std::size_t callCount) const { return Repeat(0, callCount); }
    const FunctionOptionsBase& Never() const;
    const FunctionOptionsBase& Once() const { return Repeat(1, 1); }
    const FunctionOptionsBase& Twice() const { return Repeat(2, 2); }

private:
    boost::shared_ptr<Mock> m_mock;
    boost::shared_ptr<ExpectationBase> m_expectation;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ReturnT, typename DerivedT>
class FunctionOptionsMixIn : public FunctionOptionsBase
{
public:
    typedef ReturnT ReturnType;
    typedef DerivedT DerivedType;
    typedef ReturnTypeSpecificExpectation<ReturnType> ExpectationType;

    explicit FunctionOptionsMixIn(const boost::shared_ptr<ExpectationBase>& expectation)
        :	FunctionOptionsBase(expectation)
    {
    }

    const DerivedType& Do(const typename ExpectationType::CustomReturnValueFunction& returnValueFunction) const
    {
        RUDE_MOCKS_ASSERT(!returnValueFunction.empty());
        boost::dynamic_pointer_cast<ExpectationType>(GetExpectation())->SetReturnValueFunction(returnValueFunction);
        return GetAsDerived(*this);
    }

    const DerivedType& IgnoreArguments() const { return GetAsDerived(FunctionOptionsBase::IgnoreArguments()); }
    const DerivedType& Repeat(std::size_t minCallCount, std::size_t maxCallCount) const { return GetAsDerived(FunctionOptionsBase::Repeat(minCallCount, maxCallCount)); }
    const DerivedType& Repeat(std::size_t callCount) const { return GetAsDerived(FunctionOptionsBase::Repeat(callCount)); }
    const DerivedType& Never() const { return GetAsDerived(FunctionOptionsBase::Never()); }
    const DerivedType& Once() const { return GetAsDerived(FunctionOptionsBase::Once()); }
    const DerivedType& Twice() const { return GetAsDerived(FunctionOptionsBase::Twice()); }

    template <typename ExceptionT>
    const DerivedType& Throw(ExceptionT exception) const
    {
        boost::dynamic_pointer_cast<ExpectationType>(GetExpectation())->SetReturnValueFunction(ThrowExceptionFunctionObject<ExceptionT>(exception));
        return GetAsDerived(*this);
    }

private:
    static const DerivedType& GetAsDerived(const FunctionOptionsBase& functionOptions) { return static_cast<const DerivedType&>(functionOptions); };

    template <typename ExceptionT>
    class ThrowExceptionFunctionObject
    {
    public:
        explicit ThrowExceptionFunctionObject(ExceptionT exception)
            :   m_exception(exception)
        {}

        ReturnT operator () (const boost::shared_ptr<const ExpectationBase>& expectation,
            const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments) const
        {
            throw m_exception;
        }

    private:
        ExceptionT m_exception;
    };
};

/////////////////////////////////////////////////////////////////////////////

template <typename ReturnT>
class FunctionOptions : public FunctionOptionsMixIn<ReturnT, FunctionOptions<ReturnT> >
{
private:
    typedef FunctionOptionsMixIn<ReturnT, FunctionOptions<ReturnT> > BaseType;
    typedef typename BaseType::ExpectationType ExpectationType;

    class ReturnValueFunctionObject
    {
    public:
        explicit ReturnValueFunctionObject(ReturnT returnValue)
            :   m_returnValue(returnValue)
        {}

        ReturnT operator () (const boost::shared_ptr<const ExpectationBase>& /*expectation*/,
            const boost::shared_ptr<const FunctionArgumentsBase>& /*functionArguments*/) const
        {
            return m_returnValue;
        }

    private:
        ReturnT m_returnValue;
    };

public:
    typedef ReturnT ReturnType;

    explicit FunctionOptions(const boost::shared_ptr<ExpectationBase>& expectation)
        :	FunctionOptionsMixIn<ReturnT, FunctionOptions<ReturnT> >(expectation)
    {
    }

    const FunctionOptions<ReturnT>& Return(ReturnT returnValue) const
    {
        boost::shared_ptr<ExpectationType> expectation(boost::dynamic_pointer_cast<ExpectationType>(FunctionOptionsBase::GetExpectation()));
        expectation->SetReturnValueFunction(ReturnValueFunctionObject(returnValue));
        return *this;
    }
};

/////////////////////////////////////////////////////////////////////////////

template <>
class FunctionOptions<void> : public FunctionOptionsMixIn<void, FunctionOptions<void> >
{
public:
    typedef void ReturnType;

    explicit FunctionOptions(const boost::shared_ptr<ExpectationBase>& expectation)
        :	FunctionOptionsMixIn<void, FunctionOptions<void> >(expectation)
    {
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksFunctionOptions_INCLUDED
