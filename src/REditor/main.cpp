#include "MainWindow.h"
#include <QApplication>
#include <QTableView>

#include <rconsole.h>
#include <rmodel.h>
#include <csvmodel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RConsole r(true);

    //Load data from csv
    CsvModel model;
    model.load("data.csv", ';', true);
    r["input"] = RObject::fromModel(&model);

    MainWindow w(&r);
    w.show();

    return a.exec();
}
