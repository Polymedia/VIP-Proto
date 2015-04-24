#include <QCoreApplication>

#include <httpserver/httplistener.h>
#include <rconsole.h>

#include "RequestHandler.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RConsole r(true);

    QString configFileName("listenerSettings.ini");
    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &a);
    listenerSettings->beginGroup("listener");
    HttpListener *listener = new HttpListener(listenerSettings, new RequestHandler(&r, &a) ,&a);

    return a.exec();
}

