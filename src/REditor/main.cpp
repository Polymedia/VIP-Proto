#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

#include <RInside/rconsole.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RConsole r(true);

    MainWindow w(r);
    w.showmebaby();

    return a.exec();
}
