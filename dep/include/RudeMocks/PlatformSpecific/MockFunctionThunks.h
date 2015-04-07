/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocks_PlatformSpecific_MockFunctionThunks_INCLUDED
#define RudeMocks_PlatformSpecific_MockFunctionThunks_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/VtableHelpers.h>

namespace RudeMocks
{
namespace PlatformSpecific
{

VtablePtr GetMockFunctionThunkVtable();

#if !defined(RUDE_MOCKS_USE_COMPILER_HOOKS)
FunctionAddress GetStaticallyBoundMemberFunctionThunk();
FunctionAddress GetFreeFunctionThunk();
#endif

}   // namespace PlatformSpecific
}   // namespace RudeMocks

#endif  // RudeMocks_PlatformSpecific_MockFunctionThunks_INCLUDED
