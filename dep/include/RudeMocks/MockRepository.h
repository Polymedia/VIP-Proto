/////////////////////////////////////////////////////////////////////////////
// This file is part of the RudeMocks open source project.
// (C) Copyright 2007-2012 Martin Ecker. 
//
// Use, modification and distribution is subject to the RudeMocks license. 
//   (See accompanying file license.txt or license.html)
// For more information, see http://rudemocks.sourceforge.net
/////////////////////////////////////////////////////////////////////////////

#ifndef RudeMocksMockRepository_INCLUDED
#define RudeMocksMockRepository_INCLUDED

#include <RudeMocks/Common.h>

#ifdef RUDE_MOCKS_USE_PRAGMA_ONCE
#pragma once
#endif

#include <RudeMocks/Mock.h>
#include <RudeMocks/ActualFunctionCall.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace RudeMocks
{

class IVerificationFailureHandler;
class IExpectationSectionFactory;
class ExpectationSectionBase;
class UnorderedExpectationSection;

/////////////////////////////////////////////////////////////////////////////
/*  The mock repository class acts as both a container and a factory
    for all mocks. */

class MockRepository : private boost::noncopyable
{
public:
    enum State
    {
        StateRecord,
        StateReplay,
        StateVerified
    };

    MockRepository();
    explicit MockRepository(const boost::shared_ptr<IExpectationSectionFactory>& expectationSectionFactory);	// used for unit tests
    ~MockRepository();

    template <typename T>
    T* CreateClassMock(T& object, Mock::IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode = Mock::DontIgnoreUnexpectedCalls)
    {
        boost::shared_ptr<ClassMock<T> > mock(new ClassMock<T>(*this, ignoreUnexpectedCallsMode, object));
        m_classMocks.push_back(mock);
        return mock->GetMockedObject()->GetMockedObjectPointer();
    }

    template <typename T>
    T* CreateClassMock(Mock::IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode = Mock::DontIgnoreUnexpectedCalls)
    {
        boost::shared_ptr<ClassMock<T> > mock(new ClassMock<T>(*this, ignoreUnexpectedCallsMode));
        m_classMocks.push_back(mock);
        return mock->GetMockedObject()->GetMockedObjectPointer();
    }

    FreeFunctionMock* CreateFreeFunctionMock(Mock::IgnoreUnexpectedCallsMode ignoreUnexpectedCallsMode = Mock::DontIgnoreUnexpectedCalls)
    {
        boost::shared_ptr<FreeFunctionMock> mock(new FreeFunctionMock(*this, ignoreUnexpectedCallsMode));
        m_freeFunctionMocks.push_back(mock);
        return mock.get();
    }

    State GetState() const { return m_state; }
    void Replay();
    void Verify();

    void Ordered();
    void Unordered();
    bool IsOrdered() const { return m_isOrdered; }

    static bool IsInstantiated() { return sm_instance != NULL; }
    static const boost::shared_ptr<Mock>& GetLastCalledMock() { return sm_lastCalledMock; }
    static void SetLastCalledMock(const boost::shared_ptr<Mock>& mock) { sm_lastCalledMock = mock; }
    std::size_t GetClassMockCount() const { return m_classMocks.size(); }
    std::size_t GetFreeFunctionMockCount() const { return m_freeFunctionMocks.size(); }
    static FreeFunctionMock* GetRawFreeFunctionMock(MockFunctionKey key);       // called from the free function mock thunk

    void SetVerificationFailureHandler(const boost::shared_ptr<IVerificationFailureHandler>& verificationFailureHandler);
    const boost::shared_ptr<IVerificationFailureHandler>& GetVerificationFailureHandler() const { return m_verificationFailureHandler; }
    const boost::shared_ptr<IExpectationSectionFactory>& GetExpectationSectionFactory() const { return m_expectationSectionFactory; }

    void AddExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    const boost::shared_ptr<ExpectationBase>& GetLastExpectation() const { return m_lastExpectation; }
    std::size_t GetExpectationCount() const;
    std::size_t GetExpectationSectionCount() const { return m_expectationSections.size(); }
    const boost::shared_ptr<ExpectationSectionBase>& GetExpectationSection(std::size_t index) const { RUDE_MOCKS_ASSERT(index < GetExpectationSectionCount()); return m_expectationSections[index]; }

    void AddNeverExpectation(const boost::shared_ptr<ExpectationBase>& expectation);
    std::size_t GetNeverExpectationCount() const;
    bool ContainsNeverExpectation(const boost::shared_ptr<ExpectationBase>& expectation) const;

    void AddStubbedCall(const boost::shared_ptr<ExpectationBase>& expectation);
    std::size_t GetStubbedCallCount() const;
    bool ContainsStubbedCall(const boost::shared_ptr<ExpectationBase>& expectation) const;

    boost::any AddAndVerifyActualFunctionCall(const ActualFunctionCall& actualFunctionCall);
    std::size_t GetActualFunctionCallCount() const { return m_actualFunctionCalls.size(); }
    const ActualFunctionCall& GetActualFunctionCall(std::size_t index) const { RUDE_MOCKS_ASSERT(index < m_actualFunctionCalls.size()); return m_actualFunctionCalls[index]; }
    bool ShouldFailIfCalled(const ActualFunctionCall& actualFunctionCall) const;

private:
    void AddExpectationSection();

private:
    static MockRepository* sm_instance;

    State m_state;
    bool m_isOrdered;
    
    static boost::shared_ptr<Mock> sm_lastCalledMock;
    std::vector<boost::shared_ptr<ClassMockBase> > m_classMocks;
    std::vector<boost::shared_ptr<FreeFunctionMock> > m_freeFunctionMocks;

    boost::shared_ptr<IVerificationFailureHandler> m_verificationFailureHandler;

    boost::shared_ptr<IExpectationSectionFactory> m_expectationSectionFactory;
    typedef std::vector<boost::shared_ptr<ExpectationSectionBase> > ExpectationSectionContainer;
    ExpectationSectionContainer m_expectationSections;
    ExpectationSectionContainer::iterator m_currentExpectationSection;

    boost::shared_ptr<UnorderedExpectationSection> m_neverExpectations;
    boost::shared_ptr<UnorderedExpectationSection> m_stubbedCalls;

    boost::shared_ptr<ExpectationBase> m_lastExpectation;

    typedef std::vector<ActualFunctionCall> ActualFunctionCallContainer;
    ActualFunctionCallContainer m_actualFunctionCalls;
};

}   // namespace RudeMocks

#endif  // RudeMocksMockRepository_INCLUDED
