/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksExpectationSection_INCLUDED
#define RudeMocksExpectationSection_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/MockFunctionKey.h>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <vector>
#include <map>

namespace RudeMocks
{

class IVerificationFailureHandler;
class ExpectationBase;
class ActualFunctionCall;
class UnorderedExpectationSection;

/////////////////////////////////////////////////////////////////////////////
/*  The abstract base class for all expectation sections. An expectation
    section is a collection of expectations that share a common mock
    repository state, in particular whether expectations need to be
    verified in order or can be verified in any order and whether
    unexpected calls should be ignored or not. */

class ExpectationSectionBase
{
public:
    virtual ~ExpectationSectionBase() {}

    virtual std::size_t GetExpectationCount() const = 0;
    virtual void AddExpectation(const boost::shared_ptr<ExpectationBase>& expectation) = 0;
    virtual void RemoveExpectation(const boost::shared_ptr<ExpectationBase>& expectation) = 0;
    virtual bool ContainsExpectation(const boost::shared_ptr<ExpectationBase>& expectation) const = 0;
    
    virtual bool VerifyNextActualFunctionCall(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
        const ActualFunctionCall& actualFunctionCall, const boost::shared_ptr<UnorderedExpectationSection>& stubbedCalls,
        boost::any& returnValue) = 0;
    virtual void Verify(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler) = 0;

protected:
    struct ExpectationData
    {
        ExpectationData()
            :   m_successfullyVerifiedCount(0)
        {}

        explicit ExpectationData(const boost::shared_ptr<ExpectationBase>& expectation)
            :   m_expectation(expectation), m_successfullyVerifiedCount(0)
        {}

        const boost::shared_ptr<ExpectationBase>& GetExpectation() const { return m_expectation; }
        std::size_t GetSuccessfullyVerifiedCount() const { return m_successfullyVerifiedCount; }
        void IncrementSuccessfullyVerifiedCount();
        bool IsSuccessfullyVerified() const;

    private:
        boost::shared_ptr<ExpectationBase> m_expectation;
        std::size_t m_successfullyVerifiedCount;
    };
};

/////////////////////////////////////////////////////////////////////////////

class OrderedExpectationSection : public ExpectationSectionBase
{
public:
    OrderedExpectationSection()
        :   m_expectationIndex(0)
    {}

    std::size_t GetExpectationIndex() const { return m_expectationIndex; }      // mostly for unit tests

    virtual std::size_t GetExpectationCount() const { return m_expectations.size(); }
    virtual void AddExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    virtual void RemoveExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    virtual bool ContainsExpectation(const boost::shared_ptr<ExpectationBase>& expectation) const;

    virtual bool VerifyNextActualFunctionCall(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
        const ActualFunctionCall& actualFunctionCall, const boost::shared_ptr<UnorderedExpectationSection>& stubbedCalls,
        boost::any& returnValue);
    virtual void Verify(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler);

private:
    typedef std::vector<ExpectationData> ExpectationContainer;
    ExpectationContainer m_expectations;
    std::size_t m_expectationIndex;
};

/////////////////////////////////////////////////////////////////////////////

class UnorderedExpectationSection : public ExpectationSectionBase
{
public:
    UnorderedExpectationSection()
        :   m_verifiedExpectationCount(0)
    {}
   
    std::size_t GetSuccessfullyVerifiedExpectationCount() const;

    virtual std::size_t GetExpectationCount() const { return m_expectations.size(); }
    virtual void AddExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    virtual void RemoveExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    virtual bool ContainsExpectation(const boost::shared_ptr<ExpectationBase>& expectation) const;

    bool AttemptToVerifyActualFunctionCall(const ActualFunctionCall& actualFunctionCall, boost::any& returnValue);

    virtual bool VerifyNextActualFunctionCall(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler,
        const ActualFunctionCall& actualFunctionCall, const boost::shared_ptr<UnorderedExpectationSection>& stubbedCalls,
        boost::any& returnValue);
    virtual void Verify(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler);

private:
    ExpectationData* GetExpectationData(const ActualFunctionCall& actualFunctionCall);

private:
    typedef std::multimap<MockFunctionKey, ExpectationData> ExpectationContainer;
    ExpectationContainer m_expectations;
    std::size_t m_verifiedExpectationCount;
};

}   // namespace RudeMocks

#endif  // RudeMocksExpectationSection_INCLUDED
