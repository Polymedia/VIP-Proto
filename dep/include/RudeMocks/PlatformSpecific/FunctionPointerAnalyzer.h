/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocks_PlatformSpecific_FunctionPointerAnalyzer_INCLUDED
#define RudeMocks_PlatformSpecific_FunctionPointerAnalyzer_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/VtableHelpers.h>
#include <RudeMocks/FunctionSignature.h>
#include <boost/type_traits/is_member_function_pointer.hpp>
#include <boost/type_traits/is_base_of.hpp>

namespace RudeMocks
{
namespace PlatformSpecific
{

/////////////////////////////////////////////////////////////////////////////
/*  Small data class that stores information about a member function
    pointer. Use the static Create function to conveniently create instances 
    of this class. */

class MemberFunctionPointerInfo
{
public:
    MemberFunctionPointerInfo(FunctionAddress functionAddress, bool isVirtual, VtableIndex vtableIndex, std::ptrdiff_t thisAdjustment)
        :   m_functionAddress(functionAddress), m_isVirtual(isVirtual), m_vtableIndex(vtableIndex), m_thisAdjustment(thisAdjustment)
    {}

    template <typename MemberFunctionPointerT>
    static MemberFunctionPointerInfo Create(MemberFunctionPointerT memberFunctionPointer)
    {
        RUDE_MOCKS_STATIC_ASSERT(boost::is_member_function_pointer<MemberFunctionPointerT>::value);
        return Create(&memberFunctionPointer, sizeof(MemberFunctionPointerT));
    }

    FunctionAddress GetFunctionAddress() const { return m_functionAddress; }
    bool IsVirtual() const { return m_isVirtual; }
    VtableIndex GetVtableIndex() const { return m_vtableIndex; }
    std::ptrdiff_t GetThisAdjustment() const { return m_thisAdjustment; }

private:
    static MemberFunctionPointerInfo Create(const void* functionPointer, std::size_t functionPointerSize);

private:
    FunctionAddress m_functionAddress;
    bool m_isVirtual;
    VtableIndex m_vtableIndex;
    std::ptrdiff_t m_thisAdjustment;
};

/////////////////////////////////////////////////////////////////////////////
/*  Given an object address and a member function pointer to one of its
    member functions, analyzes the pointer and returns various useful
    bits of information about it. */

template <typename ObjectT, typename MemberFunctionPointerT>
class MemberFunctionPointerAnalyzer
{
public:
    typedef MemberFunctionPointerT MemberFunctionPointerType;
    typedef ObjectT ObjectType;
    
    typedef FunctionSignature<MemberFunctionPointerType> MemberFunctionSignatureType;
    typedef typename MemberFunctionSignatureType::ObjectType PotentialBaseType;     // either a base type of ObjectT or ObjectT itself
    RUDE_MOCKS_STATIC_ASSERT((boost::is_base_of<PotentialBaseType, ObjectType>::value));

    MemberFunctionPointerAnalyzer(const ObjectType* object, const PotentialBaseType* potentialObjectBase, MemberFunctionPointerType memberFunctionPointer)
        :   m_object(object), m_potentialObjectBase(potentialObjectBase), m_memberFunctionPointerInfo(MemberFunctionPointerInfo::Create(memberFunctionPointer))
    {
        RUDE_MOCKS_ASSERT(object);
        RUDE_MOCKS_ASSERT(potentialObjectBase);
        RUDE_MOCKS_ASSERT(memberFunctionPointer);
    }

    const MemberFunctionPointerInfo& GetInfo() const { return m_memberFunctionPointerInfo; }

    std::ptrdiff_t GetThisAdjustment() const
    {
        const boost::uint8_t* derivedAddress = reinterpret_cast<const boost::uint8_t*>(m_object);
        const boost::uint8_t* baseAddress = reinterpret_cast<const boost::uint8_t*>(m_potentialObjectBase);
        RUDE_MOCKS_ASSERT(derivedAddress <= baseAddress);
        std::ptrdiff_t thisAdjustmentToBase = baseAddress - derivedAddress;

        return thisAdjustmentToBase + m_memberFunctionPointerInfo.GetThisAdjustment();
    }

private:
    const ObjectType* m_object;
    const PotentialBaseType* m_potentialObjectBase;
    MemberFunctionPointerInfo m_memberFunctionPointerInfo;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ObjectT, typename MemberFunctionPointerT>
inline MemberFunctionPointerAnalyzer<ObjectT, MemberFunctionPointerT> MakeMemberFunctionPointerAnalyzer(
    const ObjectT* object, const typename FunctionSignature<MemberFunctionPointerT>::ObjectType* potentialObjectBase, 
    MemberFunctionPointerT memberFunctionPointer)
{
    return MemberFunctionPointerAnalyzer<ObjectT, MemberFunctionPointerT>(object, potentialObjectBase, memberFunctionPointer);
}

/////////////////////////////////////////////////////////////////////////////

class FreeFunctionPointerInfo
{
public:
    explicit FreeFunctionPointerInfo(FunctionAddress functionAddress)
        :   m_functionAddress(functionAddress)
    {}

    template <typename FreeFunctionPointerT>
    static FreeFunctionPointerInfo Create(FreeFunctionPointerT freeFunctionPointer)
    {
        return Create(&freeFunctionPointer, sizeof(FreeFunctionPointerT));
    }

    FunctionAddress GetFunctionAddress() const { return m_functionAddress; }

private:
    static FreeFunctionPointerInfo Create(const void* functionPointer, std::size_t functionPointerSize);

private:
    FunctionAddress m_functionAddress;
};

/////////////////////////////////////////////////////////////////////////////

template <typename FreeFunctionPointerT>
class FreeFunctionPointerAnalyzer
{
public:
    typedef FreeFunctionPointerT FreeFunctionPointerType;
    typedef FunctionSignature<FreeFunctionPointerType> FreeFunctionSignatureType;

    explicit FreeFunctionPointerAnalyzer(FreeFunctionPointerType freeFunctionPointer)
        :   m_freeFunctionPointerInfo(FreeFunctionPointerInfo::Create(freeFunctionPointer))
    {
        RUDE_MOCKS_ASSERT(freeFunctionPointer);
    }

    const FreeFunctionPointerInfo& GetInfo() const { return m_freeFunctionPointerInfo; }

private:
    FreeFunctionPointerInfo m_freeFunctionPointerInfo;
};

/////////////////////////////////////////////////////////////////////////////

template <typename FreeFunctionPointerT>
inline FreeFunctionPointerAnalyzer<FreeFunctionPointerT> MakeFreeFunctionPointerAnalyzer(FreeFunctionPointerT freeFunctionPointer)
{
    return FreeFunctionPointerAnalyzer<FreeFunctionPointerT>(freeFunctionPointer);
}

}   // namespace PlatformSpecific
}   // namespace RudeMocks

#endif  // RudeMocks_PlatformSpecific_FunctionPointerAnalyzer_INCLUDED
