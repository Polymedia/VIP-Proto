#include <UnitTest.h>

class tst_RServer : public QObject
{
    Q_OBJECT
public:
    tst_RServer();

private slots:
    void myTest();

};

tst_RServer::tst_RServer()
{

}

void tst_RServer::myTest()
{    
    // ...
}

DECLARE_TEST(tst_RServer)

#include "tst_RServer.moc"
