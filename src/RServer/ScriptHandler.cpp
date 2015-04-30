#include "ScriptHandler.h"

#include <QFile>

#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <rconsole.h>
#include <csvmodel.h>
#include <rmodel.h>


#include "controllers/JsonTableController.h"

ScriptHandler::ScriptHandler(RConsole *r, QObject *parent):
    QObject(parent),
    m_rconsole(*r),
    m_jsonTableController(new JsonTableController)
{
    m_jsonTableController->loadData();

    m_inputRObject.name = INPUT_FROM_DASHBOARD1_NAME;

    loadDataFromCSV();

    connect(&m_rconsole, SIGNAL(error(QString)), SLOT(onErr(QString)));
    connect(&m_rconsole, SIGNAL(write(QString)), SLOT(onOut(QString)));
}

ScriptHandler::~ScriptHandler()
{
    delete m_jsonTableController;
    delete m_inputRObject.robj;
}

void ScriptHandler::loadDataFromCSV(const QString &fileName)
{
    CsvModel model;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Cannot open input data file";
        return;
    }

    model.load(&file, ';', true);
    m_rconsole[INPUT_FROM_CSV_NAME] = RObject::fromModel(&model);
}

bool ScriptHandler::loadDataFromJson(const QByteArray &jsonData)
{
    RObject newObj = RObject::fromJsonObject(jsonData);

    m_inputRObject.robj = &newObj;
    m_rconsole[m_inputRObject.name] = newObj;

    return true;
}

void ScriptHandler::getResponse(HttpRequest &request, HttpResponse &response)
{
    m_jsonTableController->service(request, response);
}

QByteArray ScriptHandler::getOutputLikeJson()
{
    QJsonArray returnArr;
    QJsonObject returnObj1 = prepareOutput1();
    returnArr.append(returnObj1);

    QJsonObject returnObj2 = prepareOutput2();
    returnArr.append(returnObj2);

    QJsonObject returnObj3 = prepareOutput3();
    returnArr.append(returnObj3);

    QJsonObject returnObj4 = prepareOutput4();
    returnArr.append(returnObj4);

    QJsonObject returnObj5 = prepareOutput5();
    returnArr.append(returnObj5);

    return QJsonDocument(returnArr).toJson(QJsonDocument::Compact);
}

bool ScriptHandler::runScript(const QString &scriptName)
{
    bool ok = true;
    QFile scriptFile(scriptName);
    ok = scriptFile.open(QIODevice::ReadOnly);

    if (!ok)
        return false;

    QTextStream textStream(&scriptFile);

    QString line;
    int i = 0;
    do {
        i++;
        line = textStream.readLine();
        ok &= m_rconsole.execute(line);
    } while (!textStream.atEnd());

    return ok;
}

QJsonObject ScriptHandler::prepareOutput1()
{
    QJsonObject jsonObj;

    RObject robj = m_rconsole[OUTPUT1_NAME];
    QJsonArray headersArray, rowsArray, valuesArray;
    RObject headers = robj.attribute("names");

    for (int j = 0; j < robj.length(); j++)
        headersArray.append(QJsonValue(headers.value(j).toString()));

    for (int j = 0; j < robj.attribute("row.names").length(); j++)
        rowsArray.append(QJsonValue(robj.attribute("row.names").value(j).toString()));

    RModel firstWidgetModel(robj);
    for (int i = 0; i < firstWidgetModel.rowCount(); i++) {
        QJsonArray row;
        for (int j = 0; j < firstWidgetModel.columnCount(); j++)
            row.append(QJsonValue(firstWidgetModel.data(firstWidgetModel.index(i, j)).toDouble()));

        valuesArray.append(row);
    }

    jsonObj.insert("headers", headersArray);
    jsonObj.insert("rows", rowsArray);
    jsonObj.insert("values", valuesArray);
    jsonObj.insert("id", OUTPUT1_NAME);

    QJsonArray arr1, arr2, editMaskArr;
    for(int i = 0; i < headersArray.count(); i++)
        arr1.insert(i, 0);

    for(int i = 0; i < headersArray.count(); i++)
        arr2.insert(i, (int)(i > 2));

    editMaskArr.append(arr1);
    editMaskArr.append(arr1);
    editMaskArr.append(arr2);
    editMaskArr.append(arr2);
    editMaskArr.append(arr2);
    editMaskArr.append(arr2);

    jsonObj.insert("editMask", editMaskArr);

    return jsonObj;
}

QJsonObject ScriptHandler::prepareOutput2()
{
    QJsonObject jsonObj;

    RObject robj = m_rconsole[OUTPUT2_NAME];

    QJsonArray headersArray, rowsArray, valuesArray;
    RObject headers = robj.attribute("names");

    for (int j = 0; j < robj.length(); j++)
        headersArray.append(QJsonValue(headers.value(j).toString()));

    for (int j = 0; j < robj.attribute("row.names").length(); j++)
        rowsArray.append(QJsonValue(robj.attribute("row.names").value(j).toString()));

    RModel firstWidgetModel(robj);
    for (int i = 0; i < firstWidgetModel.rowCount(); i++) {
        QJsonArray row;
        for (int j = 0; j < firstWidgetModel.columnCount(); j++)
            row.append(QJsonValue(firstWidgetModel.data(firstWidgetModel.index(i, j)).toDouble()));

        valuesArray.append(row);
    }

    jsonObj.insert("headers", headersArray);
    jsonObj.insert("rows", rowsArray);
    jsonObj.insert("values", valuesArray);
    jsonObj.insert("id", OUTPUT2_NAME);

    return jsonObj;
}

QJsonObject ScriptHandler::prepareOutput3()
{
    QJsonObject obj;
    obj.insert("id", QJsonValue(OUTPUT3_NAME));

    QJsonArray headersArray, rowsArray, valuesArray;
    headersArray.append(QJsonValue("Header"));
    rowsArray.append(QJsonValue("Row"));

    RObject robj = m_rconsole[OUTPUT3_NAME];

    QJsonArray val;
    val.append(QJsonValue(robj.value().toInt()));
    valuesArray.append(val);

    obj.insert("rows", rowsArray);
    obj.insert("headers", headersArray);
    obj.insert("values", valuesArray);

    return obj;
}

QJsonObject ScriptHandler::prepareOutput4()
{
    QJsonObject jsonObj;
    RObject robj = m_rconsole[OUTPUT4_NAME];

    QJsonArray headersArray, rowsArray, valuesArray;
    RObject headers = robj.attribute("names");

    for (int j = 0; j < robj.length(); j++)
        headersArray.append(QJsonValue(headers.value(j).toString()));

    for (int j = 0; j < robj.attribute("row.names").length(); j++)
        rowsArray.append(QJsonValue(robj.attribute("row.names").value(j).toString()));

    RModel firstWidgetModel(robj);
    for (int i = 0; i < firstWidgetModel.rowCount(); i++) {
        QJsonArray row;
        for (int j = 0; j < firstWidgetModel.columnCount(); j++)
            row.append(QJsonValue(firstWidgetModel.data(firstWidgetModel.index(i, j)).toDouble()));

        valuesArray.append(row);
    }

    jsonObj.insert("headers", headersArray);
    jsonObj.insert("rows", rowsArray);
    jsonObj.insert("values", valuesArray);
    jsonObj.insert("id", OUTPUT4_NAME);

    return jsonObj;
}

QJsonObject ScriptHandler::prepareOutput5()
{
    QJsonObject jsonObj;

    jsonObj.insert("id", QJsonValue(OUTPUT5_NAME));

    RObject robj = m_rconsole[OUTPUT5_NAME];

    QJsonArray headersArray, rowsArray, valuesArray;
    RObject headers = robj.attribute("names");

    for (int j = 0; j < robj.length(); j++) {
        headersArray.append(QJsonValue(headers.value(j).toString()));
    }


    RModel fifthtWidgetModel(robj);
    QJsonArray val;
    for (int i = 0; i < fifthtWidgetModel.rowCount(); i++) {
        rowsArray.append(QJsonValue::fromVariant(fifthtWidgetModel.data(fifthtWidgetModel.index(i, 0))));
        val.append(QJsonValue::fromVariant(fifthtWidgetModel.data(fifthtWidgetModel.index(i, 1))));
    }

    valuesArray.append(val);

    jsonObj.insert("headers", headersArray);
    jsonObj.insert("rows", rowsArray);
    jsonObj.insert("values", valuesArray);

    return jsonObj;
}

void ScriptHandler::onErr(const QString &err)
{
    qDebug() << "ERR" << err;
}

void ScriptHandler::onOut(const QString &out)
{
    //qDebug() << "OUT" << out;
}
