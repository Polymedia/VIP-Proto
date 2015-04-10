#include "MainWindow.h"

#include <QApplication>
#include <QDir>

#include <rconsole.h>
#include <rmodel.h>
#include <csvmodel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RConsole r(true);

    //Load data from csv
    QDir inputs("inputs");
    int counter = 0;
    foreach (QString file, inputs.entryList(QStringList() << "*.csv")) {
        counter++;
        CsvModel model;
        model.load(inputs.absoluteFilePath(file), ';', true);
        r["input" + QString::number(counter)] = RObject::fromModel(&model);
    }

    MainWindow w(&r);
    w.show();

    return a.exec();
}
