/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksGetDefaultValue_INCLUDED
#define RudeMocksGetDefaultValue_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A function object that returns a default value for a given type T.
    The default implementation simply uses the default constructor. But
    for classes that don't have a default constructor this function object
    can be specialized to return a different default value.

    When used by the mocking framework T will never be a reference type
    or const-volatile qualified.
    
    T must be copy-constructible. */

template <typename T, typename EnableIf = void>
class GetDefaultValue
{
public:
    T operator () () const
    {
        return T();
    }
};

}   // namespace RudeMocks

#endif  // RudeMocksGetDefaultValue_INCLUDED
