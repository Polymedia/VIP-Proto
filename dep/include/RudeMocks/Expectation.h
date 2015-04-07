/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksExpectation_INCLUDED
#define RudeMocksExpectation_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/ArgumentOptions.h>
#include <RudeMocks/FunctionArguments.h>
#include <RudeMocks/IVerificationFailureHandler.h>
#include <RudeMocks/ArgumentValue.h>
#include <RudeMocks/ActualFunctionCall.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/mpl.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/at.hpp>
#include <vector>

namespace RudeMocks
{

class Mock;
class MockFunctionBase;

/////////////////////////////////////////////////////////////////////////////
/*  The base class for all expectations. The deriving Expectation class
    is templatized according to the function signature of the function
    for which the expectation is used. */

class ExpectationBase : public boost::enable_shared_from_this<ExpectationBase>
{
public:
    enum VerifyResult
    {
        VerifiedSuccessfully,
        VerifiedUnsuccessfully
    };

    enum VerifyMode
    {
        VerifyTest,
        VerifyNormal
    };

    ExpectationBase(const boost::shared_ptr<const MockFunctionBase>& function,
        const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments);
    virtual ~ExpectationBase();

    std::size_t GetMinCallCount() const { return m_minCallCount; }
    std::size_t GetMaxCallCount() const { return m_maxCallCount; }
    void SetCallCount(std::size_t minCallCount, std::size_t maxCallCount)
    {
        RUDE_MOCKS_ASSERT(minCallCount <= maxCallCount);
        m_minCallCount = minCallCount;
        m_maxCallCount = maxCallCount;
    }

    boost::shared_ptr<Mock> GetMock() const;
    const boost::shared_ptr<const MockFunctionBase>& GetFunction() const { return m_function; }
    const boost::shared_ptr<const FunctionArgumentsBase>& GetFunctionArguments() const { return m_functionArguments; }
    bool IsExpectedFunction(const boost::shared_ptr<const MockFunctionBase>& function) const;

    std::size_t GetArgumentVerifierCount() const { return m_argumentVerifiers.size(); }     // mostly for unit tests; should be the same as the arity of the function for which this expectation is for
    const boost::shared_ptr<const ArgumentVerifierBase>& GetArgumentVerifier(std::size_t argumentIndex) const;
    void SetArgumentVerifier(std::size_t argumentIndex, const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier);

    std::size_t GetOutArgumentValueCount() const { return m_outArgumentValues.size(); }     // mostly for unit tests; should be the same as the arity of the function for which this expectation is for
    const boost::any& GetOutArgumentValue(std::size_t argumentIndex) const;
    void SetOutArgumentValue(std::size_t argumentIndex, const boost::any& value);

    std::size_t GetArgumentOptionsCount() const { return m_argumentOptions.size(); }        // mostly for unit tests; should be the same as the arity of the function for which this expectation is for
    const boost::shared_ptr<ArgumentOptionsBase>& GetArgumentOptions(std::size_t argumentIndex) const;
    void SetArgumentOptions(std::size_t argumentIndex, const boost::shared_ptr<ArgumentOptionsBase>& argumentOptions);

    virtual boost::any GetReturnValue(const boost::shared_ptr<const FunctionArgumentsBase>& actualFunctionArguments) const = 0;
    virtual VerifyResult Verify(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
        const ActualFunctionCall& actualFunctionCall, VerifyMode verifyMode) const = 0;

    bool CanVerifySuccessfully(const ActualFunctionCall& actualFunctionCall) const;

private:
    std::size_t m_minCallCount, m_maxCallCount;
    boost::shared_ptr<const MockFunctionBase> m_function;
    boost::shared_ptr<const FunctionArgumentsBase> m_functionArguments;

    typedef std::vector<boost::shared_ptr<const ArgumentVerifierBase> > ArgumentVerifierContainer;
    ArgumentVerifierContainer m_argumentVerifiers;

    typedef std::vector<boost::any> OutArgumentValueContainer;
    OutArgumentValueContainer m_outArgumentValues;

    typedef std::vector<boost::shared_ptr<ArgumentOptionsBase> > ArgumentOptionsContainer;
    ArgumentOptionsContainer m_argumentOptions;
};

namespace Detail
{

template <typename FunctionT, typename ReturnT>
struct CallReturnValueFunction
{
    boost::any operator () (const FunctionT& returnValueFunction,
        const boost::shared_ptr<const ExpectationBase>& expectation,
        const boost::shared_ptr<const FunctionArgumentsBase>& actualFunctionArguments) const
    {
        return boost::any(returnValueFunction(expectation, actualFunctionArguments));
    }
};

template <typename FunctionT, typename ReturnT>
struct CallReturnValueFunction<FunctionT, ReturnT&>
{
    boost::any operator () (const FunctionT& returnValueFunction,
        const boost::shared_ptr<const ExpectationBase>& expectation,
        const boost::shared_ptr<const FunctionArgumentsBase>& actualFunctionArguments) const
    {
        return boost::any(boost::ref(returnValueFunction(expectation, actualFunctionArguments)));
    }
};

template <typename FunctionT>
struct CallReturnValueFunction<FunctionT, void>
{
    boost::any operator () (const FunctionT& returnValueFunction,
        const boost::shared_ptr<const ExpectationBase>& expectation,
        const boost::shared_ptr<const FunctionArgumentsBase>& actualFunctionArguments) const
    {
        returnValueFunction(expectation, actualFunctionArguments);
        return boost::any();
    }
};

}   // namespace Detail

/////////////////////////////////////////////////////////////////////////////
/*  The return type-specific expectation base class. Used to allow
    clients to specify custom return value functions. */

template <typename ReturnT>
class ReturnTypeSpecificExpectation : public ExpectationBase
{
public:
    typedef ReturnT ReturnType;

    typedef boost::function<ReturnType (
        const boost::shared_ptr<const ExpectationBase>& expectation,
        const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments
        )> CustomReturnValueFunction;

public:
    ReturnTypeSpecificExpectation(const boost::shared_ptr<const MockFunctionBase>& function,
        const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments)
        :   ExpectationBase(function, functionArguments)
    {}

    virtual boost::any GetReturnValue(const boost::shared_ptr<const FunctionArgumentsBase>& actualFunctionArguments) const
    {
        if (!GetReturnValueFunction().empty())
        {
            Detail::CallReturnValueFunction<CustomReturnValueFunction, ReturnT> callReturnValueFunction;;
            return callReturnValueFunction(GetReturnValueFunction(), shared_from_this(), actualFunctionArguments);
        }
        else
            return boost::any();
    }

    void SetReturnValueFunction(const CustomReturnValueFunction& returnValueFunction) { m_returnValueFunction = returnValueFunction; }
    const CustomReturnValueFunction& GetReturnValueFunction() const { return m_returnValueFunction; }

private:
    CustomReturnValueFunction m_returnValueFunction;
};

namespace Detail
{

template <typename ArgumentT, typename EnableIf = void>
struct ReturnOutArgumentValue
{
    void operator () (ArgumentT argument, boost::any outValue) const {}
};

template <typename ArgumentT>
struct ReturnOutArgumentValue<ArgumentT&, typename boost::enable_if<IsOutArgumentType<ArgumentT&> >::type>
{
    void operator () (ArgumentT& argument, boost::any outValue) const
    {
        argument = boost::any_cast<ArgumentT>(outValue);
    }
};

template <typename ArgumentT>
struct ReturnOutArgumentValue<ArgumentT*, typename boost::enable_if<IsOutArgumentType<ArgumentT*> >::type>
{
    void operator () (ArgumentT* argument, boost::any outValue) const
    {
        RUDE_MOCKS_ASSERT(argument);
        *argument = boost::any_cast<ArgumentT>(outValue);
    }
};

template <>
struct ReturnOutArgumentValue<void*>
{
    void operator () (void* /*argument*/, boost::any /*outValue*/) const {}
};

}   // namespace Detail

/////////////////////////////////////////////////////////////////////////////
/*  The function-specific expectation class. */

template <typename FunctionSignatureT>
class Expectation : public ReturnTypeSpecificExpectation<typename FunctionSignatureT::ReturnType>
{
public:
    typedef FunctionSignatureT FunctionSignatureType;
    typedef typename FunctionSignatureType::ArgumentVectorType ArgumentVectorType;
    typedef typename FunctionSignatureT::ReturnType ReturnType;
    typedef FunctionArguments<ArgumentVectorType> FunctionArgumentsType;

    Expectation(const boost::shared_ptr<const MockFunctionBase>& function,
        const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments)
        :   ReturnTypeSpecificExpectation<ReturnType>(function, functionArguments)
    {
        RUDE_MOCKS_ASSERT(boost::dynamic_pointer_cast<const FunctionArgumentsType>(functionArguments));
    }

    void CreateDefaultArgumentVerifiersAndOptions()
    {
        boost::mpl::for_each<typename FunctionArgumentsType::ArgumentIntegralConstantRange>(CreateDefaultArgumentVerifierAndOptions(this));
    }

private:
    class CreateDefaultArgumentVerifierAndOptions
    {
    public:
        explicit CreateDefaultArgumentVerifierAndOptions(ExpectationBase* expectation)
            :   m_expectation(expectation)
        {}

        template <typename IntT>
        void operator () (IntT) const
        {
            const int argumentIndex = IntT::value;
            typedef typename boost::mpl::at<ArgumentVectorType, IntT>::type ArgumentType;

            boost::shared_ptr<ArgumentVerifierBase> argumentVerifier(MakeArgumentVerifierPredicate<ArgumentType>(Predicate::IsEqual<ArgumentType>()));
            m_expectation->SetArgumentVerifier(argumentIndex, argumentVerifier);

            boost::shared_ptr<ArgumentOptionsBase> argumentOptions(new ArgumentOptions<ArgumentType>(m_expectation->shared_from_this(), argumentIndex));
            m_expectation->SetArgumentOptions(argumentIndex, argumentOptions);
        }

    private:
        ExpectationBase* m_expectation;
    };

    class VerifyArgument
    {
    public:
        VerifyArgument(const ExpectationBase& expectation, ExpectationBase::VerifyResult& result,
            const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
            const boost::shared_ptr<const FunctionArgumentsType>& expectedArguments,
            const boost::shared_ptr<FunctionArgumentsType>& actualArguments,
            ExpectationBase::VerifyMode verifyMode)
            :   m_expectation(expectation), m_result(result), m_verificationFailureHandler(verificationFailureHandler),
                m_expectedArguments(expectedArguments), m_actualArguments(actualArguments), m_verifyMode(verifyMode)
        {}

        template <typename IntT>
        void operator () (IntT) const
        {
            const int argumentIndex = IntT::value;
            typedef typename boost::mpl::at<ArgumentVectorType, IntT>::type ArgumentType;
            const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier = m_expectation.GetArgumentVerifier(argumentIndex);
            boost::shared_ptr<const ArgumentVerifier<ArgumentType> > downcastedArgumentVerifier =
                boost::dynamic_pointer_cast<const ArgumentVerifier<ArgumentType> >(argumentVerifier);
            RUDE_MOCKS_ASSERT(downcastedArgumentVerifier);

            std::string description;
            if (!downcastedArgumentVerifier->Verify(argumentIndex,
                ArgumentValue<ArgumentType>(m_expectedArguments->template GetArgument<argumentIndex>(), m_verifyMode == ExpectationBase::VerifyNormal),
                ArgumentValue<ArgumentType>(m_actualArguments->template GetArgument<argumentIndex>(), m_verifyMode == ExpectationBase::VerifyNormal), description))
            {
                m_verificationFailureHandler->OnArgumentVerificationFailed(
                    GetArgumentVerificationFailedDescription(m_expectation.shared_from_this()) + ' ' + description,
                    ActualFunctionCall(m_expectation.GetFunction(), m_actualArguments),
                    m_expectation.shared_from_this(), argumentIndex, argumentVerifier);
                m_result = ExpectationBase::VerifiedUnsuccessfully;
            }

            if (m_verifyMode == ExpectationBase::VerifyNormal)
            {
                const boost::any& outValue = m_expectation.GetOutArgumentValue(argumentIndex);
                if (!outValue.empty())
                {
                    RUDE_MOCKS_ASSERT(IsOutArgumentType<ArgumentType>::value);
                    Detail::ReturnOutArgumentValue<ArgumentType> returnOutArgumentValue;
                    returnOutArgumentValue(m_actualArguments->template GetArgument<argumentIndex>(), outValue);
                }
            }
        }

    private:
        const ExpectationBase& m_expectation;
        ExpectationBase::VerifyResult& m_result;
        boost::shared_ptr<IVerificationFailureHandler> m_verificationFailureHandler;
        boost::shared_ptr<const FunctionArgumentsType> m_expectedArguments;
        boost::shared_ptr<FunctionArgumentsType> m_actualArguments;
        ExpectationBase::VerifyMode m_verifyMode;
    };

public:
    virtual ExpectationBase::VerifyResult Verify(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
        const ActualFunctionCall& actualFunctionCall, ExpectationBase::VerifyMode verifyMode) const
    {
        RUDE_MOCKS_ASSERT(verificationFailureHandler);

        const boost::shared_ptr<const MockFunctionBase>& actualFunction = actualFunctionCall.GetFunction();
        const boost::shared_ptr<FunctionArgumentsBase>& actualFunctionArguments = actualFunctionCall.GetFunctionArguments();
		
        // Workaround for c1489
        boost::ignore_unused_variable_warning(actualFunction);		
		
        RUDE_MOCKS_ASSERT(actualFunction);
        RUDE_MOCKS_ASSERT(actualFunctionArguments);
        RUDE_MOCKS_ASSERT(ExpectationBase::IsExpectedFunction(actualFunction));
        RUDE_MOCKS_ASSERT(actualFunctionArguments->GetArgumentCount() == FunctionSignatureType::arity);

        boost::shared_ptr<const FunctionArgumentsType> downcastedExpectedFunctionArguments = boost::dynamic_pointer_cast<const FunctionArgumentsType>(ExpectationBase::GetFunctionArguments());
        RUDE_MOCKS_ASSERT(downcastedExpectedFunctionArguments);
        boost::shared_ptr<FunctionArgumentsType> downcastedActualFunctionArguments = boost::dynamic_pointer_cast<FunctionArgumentsType>(actualFunctionArguments);
        RUDE_MOCKS_ASSERT(downcastedActualFunctionArguments);

        ExpectationBase::VerifyResult result = ExpectationBase::VerifiedSuccessfully;
        boost::mpl::for_each<typename FunctionArgumentsType::ArgumentIntegralConstantRange>(VerifyArgument(*this, result, verificationFailureHandler,
            downcastedExpectedFunctionArguments, downcastedActualFunctionArguments, verifyMode));

        return result;
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksExpectation_INCLUDED
