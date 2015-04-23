#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QDebug>

#include <RInside/rconsole.h>
#include <RInside/rmodel.h>
#include <RInside/csvmodel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    RConsole r(true);

    //Load data from csv
    QDir inputs("inputs");
    int counter = 0;
    foreach (QString filename, inputs.entryList(QStringList() << "*.csv")) {
        counter++;
        CsvModel model;

        QFile file(inputs.absoluteFilePath(filename));
        if (!file.open(QFile::ReadOnly)) {
            qWarning() << "Cannot open input data file";
            continue;
        }

        model.load(&file, ';', true);
        r["In_" + QString::number(counter)] = RObject::fromModel(&model);
    }

    MainWindow w(&r);
    w.show();

    return a.exec();
}
