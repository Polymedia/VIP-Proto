/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker.
//
// Use, modification and distribution is subject to the RudeMocks license.
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockFunction_INCLUDED
#define RudeMocksMockFunction_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/MockFunctionKey.h>
#include <RudeMocks/FunctionRecorderAndReplayer.h>
#include <RudeMocks/Expectation.h>
#include <RudeMocks/ForceReinterpretCast.h>
#include <boost/enable_shared_from_this.hpp>

namespace RudeMocks
{

/////////////////////////////////////////////////////////////////////////////
/*  A structure that generically represents a record or replay function
    of a FunctionRecorderAndReplayer. */

class RecordOrReplayFunction
{
public:
    RecordOrReplayFunction()
        :   m_functionRecorderAndReplayer(NULL), m_functionAddress(NULL)
    {}

    template <typename FunctionPtrT>
    RecordOrReplayFunction(FunctionRecorderAndReplayerBase* functionRecorderAndReplayer, FunctionPtrT functionPtr)
        :   m_functionRecorderAndReplayer(functionRecorderAndReplayer),
            m_functionAddress(force_reinterpret_cast<FunctionAddress>(functionPtr))
    {
        RUDE_MOCKS_ASSERT(functionRecorderAndReplayer);
        RUDE_MOCKS_ASSERT(functionPtr);
    }

    FunctionRecorderAndReplayerBase* GetFunctionRecorderAndReplayer() const { return m_functionRecorderAndReplayer; }
    FunctionAddress GetFunctionAddress() const { return m_functionAddress; }

private:
    FunctionRecorderAndReplayerBase* m_functionRecorderAndReplayer;
    FunctionAddress m_functionAddress;
};

/////////////////////////////////////////////////////////////////////////////
/*  Instances of this class uniquely identify a function on a per mock
    basis. It associates a per-mock, unique key with the function and
    can return record and replay functions that are called from the
    mock function thunks to record or replay function calls. */

class MockFunctionBase : public boost::enable_shared_from_this<MockFunctionBase>
{
protected:
    MockFunctionBase(const boost::shared_ptr<Mock>& mock, MockFunctionKey key)
        :  m_mock(mock), m_key(key)
    {
        RUDE_MOCKS_ASSERT(mock);
    }

public:
    virtual ~MockFunctionBase() {}

    boost::shared_ptr<Mock> GetMock() const { return boost::shared_ptr<Mock>(m_mock); }
    MockFunctionKey GetKey() const { return m_key; }

    virtual std::string GetName() const = 0;
    virtual RecordOrReplayFunction GetRecordFunction() const = 0;
    virtual RecordOrReplayFunction GetReplayFunction() const = 0;
    virtual boost::shared_ptr<ExpectationBase> CreateExpectation(const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments) const = 0;

private:
    boost::weak_ptr<Mock> m_mock;
    MockFunctionKey m_key;
};

/////////////////////////////////////////////////////////////////////////////
/*  The actual mock function class that depends on the signature of
    the function it represents. */

template <typename FunctionSignatureT>
class MockFunction : public MockFunctionBase
{
public:
    typedef MockFunction<FunctionSignatureT> ThisType;
    typedef FunctionSignatureT FunctionSignatureType;

    MockFunction(const boost::shared_ptr<Mock>& mock, MockFunctionKey key)
        :   MockFunctionBase(mock, key), m_functionRecorderAndReplayer(mock)
    {}

    virtual std::string GetName() const
    {
        return std::string(typeid(typename FunctionSignatureType::FunctionPtrType).name());
    }

    virtual RecordOrReplayFunction GetRecordFunction() const
    {
        return RecordOrReplayFunction(&m_functionRecorderAndReplayer, &FunctionRecorderAndReplayerType::Record);
    }

    virtual RecordOrReplayFunction GetReplayFunction() const
    {
        return RecordOrReplayFunction(&m_functionRecorderAndReplayer, &FunctionRecorderAndReplayerType::Replay);
    }

    virtual boost::shared_ptr<ExpectationBase> CreateExpectation(const boost::shared_ptr<const FunctionArgumentsBase>& functionArguments) const
    {
        boost::shared_ptr<Expectation<FunctionSignatureType> > expectation(new Expectation<FunctionSignatureType>(shared_from_this(), functionArguments));
        expectation->CreateDefaultArgumentVerifiersAndOptions();
        return expectation;
    }

private:
    typedef FunctionRecorderAndReplayer<typename FunctionSignatureType::FunctionPtrType> FunctionRecorderAndReplayerType;
    mutable FunctionRecorderAndReplayerType m_functionRecorderAndReplayer;
};

}   // namespace RudeMocks

#endif  // RudeMocksMockFunction_INCLUDED
