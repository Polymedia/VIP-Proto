/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksPatchedFunctionRegistry_INCLUDED
#define RudeMocksPatchedFunctionRegistry_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/VtableHelpers.h>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <map>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A helper class used to patch a function to call into a thunk instead
    of actually executing the function. Used by the PatchedFunctionRegistry. */

class FunctionPatcher
{
private:
    friend class PatchedFunctionRegistry;
    FunctionPatcher(FunctionAddress functionAddress, FunctionAddress thunkAddress);

public:
    ~FunctionPatcher();

    void Patch();
    void Unpatch();
    bool IsPatched() const { return m_isPatched; }

private:
    FunctionAddress m_functionAddress, m_thunkAddress;
    typedef boost::array<boost::uint8_t, 5> PatchedBytesArray;
    PatchedBytesArray m_patchedBytes;
    bool m_isPatched;
};

/////////////////////////////////////////////////////////////////////////////
/*  A global registry of all function patchers that are currently active, 
    i.e. of all functions that were patched to call into a mock function
    thunk instenad of calling the real function. This is mostly used
    so that we don't double patch a function. */

class PatchedFunctionRegistry : private boost::noncopyable
{
private:
    PatchedFunctionRegistry();

public:
    static PatchedFunctionRegistry& GetInstance();

    boost::shared_ptr<FunctionPatcher> PatchFunction(FunctionAddress functionAddress, FunctionAddress thunkAddress);
    bool IsPatched(FunctionAddress functionAddress) const;
    boost::shared_ptr<FunctionPatcher> GetFunctionPatcher(FunctionAddress functionAddress) const;

private:
    void PruneFunctionPatchers();

private:
    typedef std::map<FunctionAddress, boost::weak_ptr<FunctionPatcher> > PatchedFunctionContainer;
    PatchedFunctionContainer m_patchedFunctions;
};

}   // namespace RudeMocks

#endif  // RudeMocksPatchedFunctionRegistry_INCLUDED
