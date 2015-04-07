/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksExpectationSectionFactory_INCLUDED
#define RudeMocksExpectationSectionFactory_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <boost/shared_ptr.hpp>

namespace RudeMocks
{

class OrderedExpectationSection;
class UnorderedExpectationSection;

////////////////////////////////////////////////////////////////////////////
//	A factory for expectation sections. Mostly used for unit tests.

class IExpectationSectionFactory
{
public:
    virtual ~IExpectationSectionFactory() {}

    virtual boost::shared_ptr<OrderedExpectationSection> CreateOrderedExpectationSection() const = 0;
    virtual boost::shared_ptr<UnorderedExpectationSection> CreateUnorderedExpectationSection() const = 0;
};

////////////////////////////////////////////////////////////////////////////

class ExpectationSectionFactory : public IExpectationSectionFactory
{
public:
    virtual boost::shared_ptr<OrderedExpectationSection> CreateOrderedExpectationSection() const;
    virtual boost::shared_ptr<UnorderedExpectationSection> CreateUnorderedExpectationSection() const;
};

}	// namespace RudeMocks

#endif	// RudeMocksExpectationSectionFactory_INCLUDED


