#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

#include <RInside/rconsole.h>

#include <RInside/csvmodel.h>
#include <RInside/rmodel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RConsole r(true);

    MainWindow w(r);
    w.show();





    return a.exec();
}
