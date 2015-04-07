/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksBoostTestVerificationFailureHandler_INCLUDED
#define RudeMocksBoostTestVerificationFailureHandler_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#if RUDE_MOCKS_USE_BOOST_TEST_VERIFICATION_FAILURE_HANDLER

#include <RudeMocks/IVerificationFailureHandler.h>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  This verification failure handler fails with BOOST_FAIL when an 
    expectation is not satisfied. */

class BoostTestVerificationFailureHandler : public IVerificationFailureHandler
{
public:
    virtual void OnUnexpectedFunctionCalled(const std::string& description, const ActualFunctionCall& actualFunctionCall);
    virtual void OnExpectedFunctionNotCalled(const std::string& description, const boost::shared_ptr<const ExpectationBase>& expectation);
    virtual void OnArgumentVerificationFailed(const std::string& description, const ActualFunctionCall& actualFunctionCall, 
        const boost::shared_ptr<const ExpectationBase>& expectation, 
        std::size_t argumentIndex, const boost::shared_ptr<const ArgumentVerifierBase>& argumentVerifier);
};

}   // namespace RudeMocks

#endif	// RUDE_MOCKS_USE_BOOST_TEST_VERIFICATION_FAILURE_HANDLER

#endif  // RudeMocksBoostTestVerificationFailureHandler_INCLUDED
