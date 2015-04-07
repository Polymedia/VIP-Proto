/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksNonFatalVerificationFailureHandler_INCLUDED
#define RudeMocksNonFatalVerificationFailureHandler_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/IVerificationFailureHandler.h>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A simple verification failure handler that simply records whether
    or not it was ever called. */

class NonFatalVerificationFailureHandler : public IVerificationFailureHandler
{
public:
    NonFatalVerificationFailureHandler()
        :   m_wasEverCalled(false)
    {}

    bool WasEverCalled() const { return m_wasEverCalled; }

    virtual void OnUnexpectedFunctionCalled(const std::string& /* description */, const ActualFunctionCall& /* actualFunctionCall */)
    {
        m_wasEverCalled = true;
    }
    
    virtual void OnExpectedFunctionNotCalled(const std::string& /* description */, const boost::shared_ptr<const ExpectationBase>& /* expectation */)
    {
        m_wasEverCalled = true;
    }

    virtual void OnArgumentVerificationFailed(const std::string& /* description */, const ActualFunctionCall& /* actualFunctionCall */,
        const boost::shared_ptr<const ExpectationBase>& /* expectation */, 
        std::size_t /* argumentIndex */, const boost::shared_ptr<const ArgumentVerifierBase>& /* argumentVerifier */)
    {
        m_wasEverCalled = true;
    }

private:
    bool m_wasEverCalled;
};

}   // namespace RudeMocks

#endif  // RudeMocksNonFatalVerificationFailureHandler_INCLUDED
