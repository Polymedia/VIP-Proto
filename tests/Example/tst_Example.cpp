#include <UnitTest.h>

class ITest
{
public:
    virtual void func() = 0;
};

class MockTest : public ITest
{
public:
    MOCK_METHOD0(func, void());
};

class tst_Example : public QObject
{
    Q_OBJECT
public:
    tst_Example();

private slots:
    void myTest();

};

tst_Example::tst_Example()
{

}

void tst_Example::myTest()
{    
    // ...
    MockTest mt;

    EXPECT_CALL(mt, func());

    GMOCK_VERIFY(mt);
}

DECLARE_TEST(tst_Example)

#include "tst_Example.moc"
