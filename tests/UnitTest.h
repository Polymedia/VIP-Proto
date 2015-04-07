#ifndef UNITTEST_H
#define UNITTEST_H

/** *********************************** Qt ************************************** **/
#include <QDebug>
#include <QObject>

#include <QtTest/QtTest>

/** **************************** Google Mock stuff ****************************** **/
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;

using ::testing::AtLeast;
using ::testing::NiceMock;

using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::IgnoreResult;

using ::testing::Return;

/*
 * Verifies expectations for mock object
*/
#define GMOCK_VERIFY(object) \
QVERIFY(::testing::Mock::VerifyAndClearExpectations(&object));

/*
 * Verifies expectations for mock object and prints custom message on error
*/
#define GMOCK_VERIFY2(object, message) \
QVERIFY2(::testing::Mock::VerifyAndClearExpectations(&object), message);

/*
 * Verifies expectations for mock object and if error occured prints custom message
*/
#define GMOCK_EXPECT_FAIL(object, message) \
QEXPECT_FAIL("", message, Continue); \
QVERIFY(::testing::Mock::VerifyAndClearExpectations(&object));

/** **************************** Rude Mock stuff ****************************** **/
/*
 * RudeMock is based on Boost
 * Boost and Qt has some incompatibilities and we applying this fix
 * to supress macro redefinition warnings
 *
 * For details look at:
 * https://bugreports.qt-project.org/browse/QTBUG-22829
 * https://bugreports.qt-project.org/browse/QTBUG-29331
 * https://bitbucket.org/osrf/sdformat/pull-request/14/fix-for-qt-bug-22829/diff
 *
*/
#ifndef Q_MOC_RUN
    #include <RudeMocks/RudeMocks.h>
#endif

using namespace RudeMocks;

#define RUDEMOCK_VERIFY(repo) \
try { \
    repo.Verify(); \
} catch (VerificationFailureException &e) { \
    QFAIL(e.what()); \
}

/** ********************** General unit test defines *************************** **/

#include "TestRunner.h"

// Place this macro at the end of .cpp file to declare test
#define DECLARE_TEST(className) \
static TestAdder<className> t##className(#className);

/*
 * Initializes Unit testing system
*/
#define UNITTEST_INIT(argc, argv) \
QCommandLineParser cmd; \
cmd.addHelpOption(); \
cmd.addPositionalArgument("[test1] [test2] ...", " - Specify particular test classes for run", "[test1] [test2] ..."); \
\
cmd.process(*QCoreApplication::instance()); \
\
::testing::FLAGS_gmock_catch_leaked_mocks = false; \
::testing::InitGoogleMock(&argc, argv);

/*
 * Runs all Unit tests
*/
#define UNITTEST_RUN \
return TestRunner::instance().runTests(cmd.positionalArguments());

/*
 * Implements a main() function with QCoreApplication, initializes Google Mock
 * and executes all tests in TestObject.
*/
#define UNITTEST_GUILESS_MAIN \
int main(int argc, char *argv[]) \
{ \
    QCoreApplication app(argc, argv); \
\
    UNITTEST_INIT(argc, argv); \
\
    UNITTEST_RUN; \
}

/*
 * Implements a main() function with QApplication, initializes Google Mock
 * and executes all tests in TestObject.
 *
 * NOTE: Don't forget to include
 * QT += widgets
 * in your .pro file to use this macro
*/
#define UNITTEST_MAIN \
int main(int argc, char *argv[]) \
{ \
    QApplication app(argc, argv); \
    QTEST_DISABLE_KEYPAD_NAVIGATION \
\
    UNITTEST_INIT(argc, argv); \
\
    UNITTEST_RUN; \
}

/** ********************** Unit test utilities *************************** **/
#include "UnitTestUtil.h"

#endif // UNITTEST_H
