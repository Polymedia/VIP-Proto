/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksExceptionVerificationFailureHandler_INCLUDED
#define RudeMocksExceptionVerificationFailureHandler_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/IVerificationFailureHandler.h>
#include <RudeMocks/ActualFunctionCall.h>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  The exception base class for all exceptions thrown by the
    ExceptionVerificationFailureHandler class. */

class VerificationFailureException : public std::exception
{
public:
    explicit VerificationFailureException(const std::string& description)
        :   m_description(description)
    {}
    virtual ~VerificationFailureException() throw() {}

    const std::string& GetDescription() const { return m_description; }
    virtual const char* what() const throw() { return m_description.c_str(); }

private:
    std::string m_description;
};

/////////////////////////////////////////////////////////////////////////////

class UnexpectedFunctionCalledException : public VerificationFailureException
{
public:
    UnexpectedFunctionCalledException(const std::string& description, const ActualFunctionCall& actualFunctionCall)
        :   VerificationFailureException(description), m_actualFunctionCall(actualFunctionCall)
    {}
    virtual ~UnexpectedFunctionCalledException() throw() {}

    const ActualFunctionCall& GetActualFunctionCall() const { return m_actualFunctionCall; }

private:
    ActualFunctionCall m_actualFunctionCall;
};

/////////////////////////////////////////////////////////////////////////////

class ExpectedFunctionNotCalledException : public VerificationFailureException
{
public:
    ExpectedFunctionNotCalledException(const std::string& description, const boost::shared_ptr<const ExpectationBase>& expectation)
        :   VerificationFailureException(description), m_expectation(expectation)
    {}
    virtual ~ExpectedFunctionNotCalledException() throw() {}

    const boost::shared_ptr<const ExpectationBase>& GetExpectation() const { return m_expectation; }

private:
    boost::shared_ptr<const ExpectationBase> m_expectation;
};

/////////////////////////////////////////////////////////////////////////////

class ArgumentVerificationFailedException : public VerificationFailureException
{
public:
    ArgumentVerificationFailedException(const std::string& description, const ActualFunctionCall& actualFunctionCall,
        const boost::shared_ptr<const ExpectationBase>& expectation,
        std::size_t argumentIndex, const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier)
        :   VerificationFailureException(description), m_actualFunctionCall(actualFunctionCall), m_expectation(expectation), 
            m_argumentIndex(argumentIndex), m_argumentVerifier(argumentVerifier)
    {}
    virtual ~ArgumentVerificationFailedException() throw() {}

    const ActualFunctionCall& GetActualFunctionCall() const { return m_actualFunctionCall; }
    const boost::shared_ptr<const ExpectationBase>& GetExpectation() const { return m_expectation; }
    std::size_t GetArgumentIndex() const { return m_argumentIndex; }
    const boost::shared_ptr<const ArgumentVerifierBase>& GetArgumentVerifier() const { return m_argumentVerifier; }

private:
    ActualFunctionCall m_actualFunctionCall;
    boost::shared_ptr<const ExpectationBase> m_expectation;
    std::size_t m_argumentIndex;
    boost::shared_ptr<const ArgumentVerifierBase> m_argumentVerifier;
};

/////////////////////////////////////////////////////////////////////////////
/*  This verification failure handler throws an exception whenever
    a verification failure occurs. */

class ExceptionVerificationFailureHandler : public IVerificationFailureHandler
{
public:
    virtual void OnUnexpectedFunctionCalled(const std::string& description, const ActualFunctionCall& actualFunctionCall);
    virtual void OnExpectedFunctionNotCalled(const std::string& description, const boost::shared_ptr<const ExpectationBase>& expectation);
    virtual void OnArgumentVerificationFailed(const std::string& description, const ActualFunctionCall& actualFunctionCall, 
        const boost::shared_ptr<const ExpectationBase>& expectation,
        std::size_t argumentIndex, const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier);
};

}   // namespace RudeMocks

#endif  // RudeMocksExceptionVerificationFailureHandler_INCLUDED
