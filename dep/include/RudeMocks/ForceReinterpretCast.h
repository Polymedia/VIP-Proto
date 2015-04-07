/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksForceReinterpretCast_INCLUDED
#define RudeMocksForceReinterpretCast_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  Forcefully casts the given U to a T on the bit level. Be really
    careful when using this. You have to know what you're doing and it is
    highly recommended that T and U have the same size, although there
    are some valid examples where this might not be the case.

    Uses unions instead of reinterpret_cast to avoid aliasing issues on
    certain hardware architectures and/or compilers. */

template <typename T, typename U>
const T& force_reinterpret_cast(const U& object)
{
    union CastValues
    {
        T m_t;
        U m_u;
    };
    static CastValues s_castValues;
    s_castValues.m_u = object;
    return s_castValues.m_t;
}

template <typename T, typename U>
T& force_reinterpret_cast(U& object)
{
    union CastValues
    {
        T m_t;
        U m_u;
    };
    static CastValues s_castValues;
    s_castValues.m_u = object;
    return s_castValues.m_t;
}

}   // namespace RudeMocks

#endif  // RudeMocksForceReinterpretCast_INCLUDED
