#include <UnitTest.h>

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
}

DECLARE_TEST(tst_Example)

#include "tst_Example.moc"
