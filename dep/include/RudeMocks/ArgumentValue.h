/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksArgumentValue_INCLUDED
#define RudeMocksArgumentValue_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/IsOutArgumentType.h>
#include <boost/utility/enable_if.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////

class ArgumentValueBase
{
public:
    explicit ArgumentValueBase(bool allowsSettingOutValue)
        :   m_allowsSettingOutValue(allowsSettingOutValue)
    {}

    bool AllowsSettingOutValue() const { return m_allowsSettingOutValue; }

protected:
    ~ArgumentValueBase() {}     // don't use as polymorphic base

private:
    bool m_allowsSettingOutValue;
};

/////////////////////////////////////////////////////////////////////////////
/*  This small value template class and its specializations are used
    to pass expected and actual argument values to the Do() argument
    option. It is mostly used to prevent overwriting out or in-out 
    parameters when an expectation is verfied simply to see if the verification
    succeeds, i.e. the verification does not "count" and should not change
    out values. This is particularly important if the custom function
    registered via the Do() argument option performs some computation
    on the given actual argument value in case of in-out parameters. 
    This computation would then be performed multiple times, first for
    the verify calls that simply test if verification succeeds and then
    again for the actual verification. */

template <typename ArgumentT, typename EnableIf = void>
class ArgumentValue : public ArgumentValueBase
{
public:
    ArgumentValue(ArgumentT value, bool allowsSettingOutValue)
        :   ArgumentValueBase(allowsSettingOutValue), m_value(value)
    {}

    ArgumentT GetValue() const { return m_value; }

private:
    ArgumentT m_value;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class ArgumentValue<ArgumentT&, typename boost::enable_if<IsOutArgumentType<ArgumentT&> >::type> : public ArgumentValueBase
{
public:
    ArgumentValue(ArgumentT& value, bool allowsSettingOutValue)
        :   ArgumentValueBase(allowsSettingOutValue), m_value(value)
    {}

    const ArgumentT& GetValue() { return m_value; }
    void SetOutValue(const ArgumentT& outValue)
    { 
        if (AllowsSettingOutValue())
            m_value = outValue;
    }

private:
    ArgumentT& m_value;
};

/////////////////////////////////////////////////////////////////////////////

template <typename ArgumentT>
class ArgumentValue<ArgumentT*, typename boost::enable_if<IsOutArgumentType<ArgumentT*> >::type> : public ArgumentValueBase
{
public:
    ArgumentValue(ArgumentT* value, bool allowsSettingOutValue)
        :   ArgumentValueBase(allowsSettingOutValue), m_value(value)
    {}

    const ArgumentT* GetValue() { return m_value; }
    void SetOutValue(const ArgumentT& outValue)
    { 
        if (AllowsSettingOutValue())
            *m_value = outValue;
    }

private:
    ArgumentT* m_value;
};

/////////////////////////////////////////////////////////////////////////////

template <>
class ArgumentValue<void*> : public ArgumentValueBase
{
public:
    ArgumentValue(void* value, bool /* allowsSettingOutValue */)
        :   ArgumentValueBase(false), m_value(value)
    {}

    const void* GetValue() { return m_value; }

private:
    void* m_value;
};

}   // namespace RudeMocks

#endif  // RudeMocksArgumentValue_INCLUDED

