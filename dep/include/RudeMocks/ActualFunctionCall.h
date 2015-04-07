/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksActualFunctionCall_INCLUDED
#define RudeMocksActualFunctionCall_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/shared_ptr.hpp>
#include <boost/operators.hpp>

namespace RudeMocks
{

class MockFunctionBase;
class FunctionArgumentsBase;

/////////////////////////////////////////////////////////////////////////////
/*  A data class that represents an actual function call made on a mock
    in replay mode. */

class ActualFunctionCall : public boost::totally_ordered<ActualFunctionCall>
{
public:
    ActualFunctionCall()
    {}

    ActualFunctionCall(const boost::shared_ptr<const MockFunctionBase>& function,
        const boost::shared_ptr<FunctionArgumentsBase>& functionArguments)
        :   m_function(function), m_functionArguments(functionArguments)
    {
        RUDE_MOCKS_ASSERT(function);
        RUDE_MOCKS_ASSERT(functionArguments);
    }

    const boost::shared_ptr<const MockFunctionBase>& GetFunction() const { return m_function; }
    const boost::shared_ptr<FunctionArgumentsBase>& GetFunctionArguments() const { return m_functionArguments; }

    bool operator == (const ActualFunctionCall& other) const
    {
        return GetFunction() == other.GetFunction() && GetFunctionArguments() == other.GetFunctionArguments();
    }

    bool operator < (const ActualFunctionCall& other) const
    {
        return GetFunction() < other.GetFunction() || (GetFunction() == other.GetFunction() && GetFunctionArguments() < other.GetFunctionArguments());
    }

private:
    boost::shared_ptr<const MockFunctionBase> m_function;
    boost::shared_ptr<FunctionArgumentsBase> m_functionArguments;
};

}   // namespace RudeMocks

#endif  // RudeMocksActualFunctionCall_INCLUDED
