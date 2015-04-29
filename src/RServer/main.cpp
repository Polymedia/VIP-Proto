#include <QApplication>

#include <httpserver/httplistener.h>
#include <rconsole.h>

#include "RequestHandler.h"

#include <QTableView>
#include <RModel.h>

RConsole r(true);

void showww()
{
    RObject robj = r["fifthWidget"];
    RModel *mod = new RModel(robj);

    QTableView *view = new QTableView();
    view->setModel(mod);
    view->show();


    qDebug() << "YEEES" << mod->rowCount() << mod->columnCount();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    QString configFileName("listenerSettings.ini");
    QSettings* listenerSettings = new QSettings(configFileName, QSettings::IniFormat, &a);
    listenerSettings->beginGroup("listener");
    HttpListener *listener = new HttpListener(listenerSettings, new RequestHandler(&r, &a) ,&a);


    //QTimer::singleShot(6000, &showww);


    return a.exec();
}

