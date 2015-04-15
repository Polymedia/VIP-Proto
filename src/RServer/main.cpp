#include <QCoreApplication>

#include <httpserver/httplistener.h>
#include <httpserver/httprequesthandler.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HttpListener *listener=new HttpListener(new QSettings() ,new HttpRequestHandler(&a) ,&a);

    return a.exec();
}

