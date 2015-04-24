#include "ScriptHandler.h"

#include <QFile>

#include <QDebug>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include <rconsole.h>
#include <csvmodel.h>
#include <rmodel.h>

ScriptHandler::ScriptHandler(RConsole *r, QObject *parent):
    QObject(parent),
    m_rconsole(r)
{
    loadDataFromCSV();
    fillOperatorRObject();
}

ScriptHandler::~ScriptHandler()
{
    delete m_operatorRObject.robj;
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
    (*m_rconsole)[INPUT_FROM_CSV_NAME] = RObject::fromModel(&model);
}

bool ScriptHandler::loadDataFromJson(const QByteArray &jsonData)
{
    RObject newObj = RObject::fromJsonObject(jsonData);

    if (newObj.columns() == m_operatorRObject.robj->columns()
            && newObj.rows() == m_operatorRObject.robj->rows()) {
        (*m_rconsole)[m_operatorRObject.name] = newObj;

        return true;
    }

    return false;
}

QByteArray ScriptHandler::getOutputLikeJson()
{
    QJsonArray returnArr;
    QJsonObject returnObj1 = prepareOutput1();
    returnArr.append(returnObj1);

    return QJsonDocument(returnArr).toJson();
}

bool ScriptHandler::runScript(const QString &scriptName)
{
    RObject firstWidget(RObject::Frame, RObject::Float, 6, 7);
    firstWidget.fill(-2);

    RObject headers = firstWidget.attribute("names");
    for (int i = 0; i < headers.length(); i++)
        headers.setValue(QString("col %1").arg(i + 1), i);

    RObject rows = firstWidget.attribute("row.names");
    for (int i = 0; i < rows.length(); i++)
        rows.setValue(QString("row %1").arg(i + 1), i);

    RObject obj = (*m_rconsole)[m_operatorRObject.name];
    RModel firstWidgetModel(firstWidget);
    RModel objModel(obj);

    for (int i = 0; i < objModel.rowCount(); i++) {
        //QDebug log = qDebug();
        for (int j = 0; j < objModel.columnCount(); j++) {
            //log << objModel.data(objModel.index(i, j)).toDouble();
            firstWidgetModel.setData(firstWidgetModel.index(2 + i, 3 + j),
                                     objModel.data(objModel.index(i, j)).toDouble());
        }
    }

//    for (int i = 0; i < firstWidgetModel.rowCount(); i++) {
//        QDebug log = qDebug();
//        for (int j = 0; j < firstWidgetModel.columnCount(); j++) {
//            log << firstWidgetModel.data(firstWidgetModel.index(i, j)).toDouble();
//        }
//    }

    (*m_rconsole)[OUTPUT1_NAME] = firstWidget;

    return true;
}

void ScriptHandler::fillOperatorRObject()
{
    m_operatorRObject.name = OPERATOR_OBJECT_NAME;
    RObject *obj = new RObject(RObject::Frame, RObject::Float, 4, 4);
    obj->fill(-1);
    m_operatorRObject.robj = obj;
    (*m_rconsole)[m_operatorRObject.name] = *m_operatorRObject.robj;
}

QJsonObject ScriptHandler::prepareOutput1()
{
    QJsonObject jsonObj;

    RObject robj = (*m_rconsole)[OUTPUT1_NAME];
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

