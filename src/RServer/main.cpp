#include <QCoreApplication>

#include <httpserver/httplistener.h>

#include "RequestHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString configFileName("listenerSettings.ini");
    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &a);
    listenerSettings->beginGroup("listener");
    HttpListener *listener = new HttpListener(listenerSettings, new RequestHandler(&a) ,&a);

    return a.exec();
}

