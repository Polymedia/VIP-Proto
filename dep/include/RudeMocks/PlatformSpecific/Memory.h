/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocks_PlatformSpecific_Memory_INCLUDED
#define RudeMocks_PlatformSpecific_Memory_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

namespace RudeMocks
{
namespace PlatformSpecific
{

void UnprotectCodeMemoryRegion(void* address, std::size_t byteCount);
void ProtectCodeMemoryRegion(void* address, std::size_t byteCount);
void FlushInstructionCache(void* address, std::size_t byteCount);

}   // namespace RudeMocks
}   // namespace PlatformSpecific

#endif  // RudeMocks_PlatformSpecific_Memory_INCLUDED
