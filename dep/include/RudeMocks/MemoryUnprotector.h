/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMemoryUnprotector_INCLUDED
#define RudeMocksMemoryUnprotector_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  RAII class to unprotect a certain region of virtual memory to
    allow for self-modifying code. */

class MemoryUnprotector
{
public:
    MemoryUnprotector(void* address, std::size_t byteCount);
    ~MemoryUnprotector();

private:
    void* m_address;
    std::size_t m_byteCount;
    boost::uint32_t m_oldProtect;
};

}   // namespace RudeMocks

#endif  // RudeMocksMemoryUnprotector_INCLUDED
