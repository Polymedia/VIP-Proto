#include <UnitTest.h>

#include <csvmodel.h>

#include <QBuffer>
#include <QVector>
#include <QMetaType>

Q_DECLARE_METATYPE(QMetaType::Type)

class tst_RInside : public QObject
{
    Q_OBJECT
public:
    tst_RInside();

private slots:
    void csvModel_load_delim_data();
    void csvModel_load_delim();
    void csvModel_load_header_data();
    void csvModel_load_header();
    void csvModel_load_types_data();
    void csvModel_load_types();
    void csvModel_load_empty_data();
    void csvModel_load_empty();
    void csvModel_load_corrupted_data();
    void csvModel_load_corrupted();

};

tst_RInside::tst_RInside()
{
}

void tst_RInside::csvModel_load_delim_data()
{
    QTest::addColumn<char>("delim");
    QTest::addColumn<QString>("data");

    QTest::newRow(";") << ';' << "hello,world;10;10.5;true";
    QTest::newRow(",") << ',' << "hello.world,10,10.5,true";
    QTest::newRow(".") << '.' << "hello,world.10.10,5.true";
}

void tst_RInside::csvModel_load_delim()
{
    QFETCH(char, delim);
    QFETCH(QString, data);

    QBuffer file;
    file.setData(data.toLocal8Bit());
    file.open(QIODevice::ReadOnly);

    CsvModel model;
    model.load(&file, delim, false);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.columnCount() == 4);
}

void tst_RInside::csvModel_load_header_data()
{
    QTest::addColumn<QString>("data");

    QTest::newRow("same") << "hello;world\n100;500";
    QTest::newRow("less") << "hello\n100;500";
    QTest::newRow("more") << "hello;programming;world\n100;500";
}

void tst_RInside::csvModel_load_header()
{
    QFETCH(QString, data);

    QBuffer file;
    file.setData(data.toLocal8Bit());
    file.open(QIODevice::ReadOnly);

    CsvModel model;
    model.load(&file, ';', true);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.columnCount() == 2);
    QVERIFY(model.headerData(0, Qt::Horizontal).type() == QMetaType::QString);
    QVERIFY(model.headerData(1, Qt::Horizontal).type() == QMetaType::QString);
}

void tst_RInside::csvModel_load_types_data()
{
    QTest::addColumn<QString>("data");
    QTest::addColumn<QMetaType::Type>("type");

    QTest::newRow("bool1") << "true" << QMetaType::Bool;
    QTest::newRow("bool2") << "false" << QMetaType::Bool;
    QTest::newRow("bool3") << "True" << QMetaType::QString;
    QTest::newRow("bool4") << "False" << QMetaType::QString;
    QTest::newRow("bool5") << "yes" << QMetaType::QString;
    QTest::newRow("bool6") << "no" << QMetaType::QString;
    QTest::newRow("int1") << "1" << QMetaType::Int;
    QTest::newRow("int2") << "0" << QMetaType::Int;
    QTest::newRow("int3") << "0xFF" << QMetaType::QString;
    QTest::newRow("int4") << "026" << QMetaType::Int;
    QTest::newRow("int5") << "100500" << QMetaType::Int;
    QTest::newRow("double1") << "20.0" << QMetaType::Double;
    QTest::newRow("double2") << "20.5" << QMetaType::Double;
    QTest::newRow("double3") << "20,0" << QMetaType::QString;
    QTest::newRow("string1") << "hello" << QMetaType::QString;
}

void tst_RInside::csvModel_load_types()
{   
    QFETCH(QString, data);
    QFETCH(QMetaType::Type, type);

    QBuffer file;
    file.setData(data.toLocal8Bit());
    file.open(QIODevice::ReadOnly);

    CsvModel model;
    model.load(&file, ';', false);

    QVERIFY(model.rowCount() == 1);
    QVERIFY(model.columnCount() == 1);

    QVERIFY(model.data(model.index(0, 0)).type() == type);
}

void tst_RInside::csvModel_load_empty_data()
{
    QTest::addColumn<int>("rows");
    QTest::addColumn<QString>("data");

    QTest::newRow("empty") << 0 << "\n";
    QTest::newRow("spaces") << 3 << "100\n 200\n\t300";
    QTest::newRow("empty_space") << 0 << "\n \t\n";
}

void tst_RInside::csvModel_load_empty()
{
    QFETCH(int, rows);
    QFETCH(QString, data);

    QBuffer file;
    file.setData(data.toLocal8Bit());
    file.open(QIODevice::ReadOnly);

    CsvModel model;
    model.load(&file, ';', false);

    QVERIFY(model.rowCount() == rows);
}

void tst_RInside::csvModel_load_corrupted_data()
{
    QTest::addColumn<int>("columns");
    QTest::addColumn<QString>("data");
    QTest::addColumn<QList<int>>("values");

    QTest::newRow("space1") << 3 << "1;2;3\n4;;6" << QList<int>{1, 2, 3, 4, 0, 6};
    QTest::newRow("space2") << 3 << "1;;3\n4;5;6" << QList<int>{1, 0, 3, 4, 5, 6};
    QTest::newRow("space3") << 3 << "1;2;3\n4;5" << QList<int>{1, 2, 3, 4, 5, 0};
    QTest::newRow("space4") << 2 << "1;2\n4;5;6;7" << QList<int> {1, 2, 4, 5};
}

void tst_RInside::csvModel_load_corrupted()
{
    QFETCH(int, columns);
    QFETCH(QString, data);
    QFETCH(QList<int>, values);

    QBuffer file;
    file.setData(data.toLocal8Bit());
    file.open(QIODevice::ReadOnly);

    CsvModel model;
    model.load(&file, ';', false);

    QVERIFY(model.columnCount() == columns);

    int count = 0;
    for (int i = 0; i < model.rowCount(); ++i)
        for (int j = 0; j < model.columnCount(); ++j) {
            QVERIFY(model.data(model.index(i, j)).toInt() == values[count]);
            count++;
        }
}

DECLARE_TEST(tst_RInside)

#include "tst_RInside.moc"
