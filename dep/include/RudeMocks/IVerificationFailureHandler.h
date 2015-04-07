/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksIVerificationFailureHandler_INCLUDED
#define RudeMocksIVerificationFailureHandler_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/shared_ptr.hpp>
#include <string>

namespace RudeMocks
{

class Mock;
class MockFunctionBase;
class FunctionArgumentsBase;
class ExpectationBase;
class ArgumentVerifierBase;
class ActualFunctionCall;

/////////////////////////////////////////////////////////////////////////////
/*  The interface for expectation failure handlers. Functions of this
    interface are called when an expectation could not be verified
    successfully. */

class IVerificationFailureHandler
{
public:
    virtual ~IVerificationFailureHandler() {}

    virtual void OnUnexpectedFunctionCalled(const std::string& description, const ActualFunctionCall& actualFunctionCall) = 0;
    virtual void OnExpectedFunctionNotCalled(const std::string& description, const boost::shared_ptr<const ExpectationBase>& expectation) = 0;
    virtual void OnArgumentVerificationFailed(const std::string& description, const ActualFunctionCall& actualFunctionCall,
        const boost::shared_ptr<const ExpectationBase>& expectation, 
        std::size_t argumentIndex, const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier) = 0;
};

/////////////////////////////////////////////////////////////////////////////
/*  A number of free functions to more easily get description strings
    when verification fails. */

std::string GetUnexpectedFunctionCalledDescription(const ActualFunctionCall& actualFunctionCall,
    const boost::shared_ptr<const ExpectationBase>& expectation);
std::string GetUnexpectedFunctionCalledDescription(const ActualFunctionCall& actualFunctionCall);
std::string GetExpectedFunctionNotCalledDescription(const boost::shared_ptr<const ExpectationBase>& expectation);
std::string GetArgumentVerificationFailedDescription(const boost::shared_ptr<const ExpectationBase>& expectation);

}   // namespace RudeMocks

#endif  // RudeMocksIVerificationFailureHandler_INCLUDED
