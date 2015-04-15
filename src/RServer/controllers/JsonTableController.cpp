#include "JsonTableController.h"

JsonTableController::JsonTableController(QObject *parent) :
    HttpRequestHandler(parent),
    m_dataLoader(new JsonDataLoader)
{
    m_dataLoader->loadData("dataBase.txt");
}

JsonTableController::~JsonTableController()
{
    delete m_dataLoader;
}


void JsonTableController::service(HttpRequest &request, HttpResponse &response)
{
    // Set a response header
    response.setHeader("Content-Type", "application/json");

    // Return a simple HTML document
    //response.write("<html><body><b>TABLES</b></body></html>", true);

    response.write(m_dataLoader->data(), true);

    qDebug() << response.getHeaders();
}

void JsonTableController::service(HttpRequest &request, HttpResponse &response, QByteArrayList pathList)
{
//    // Set a response header
//    response.setHeader("Content-Type", "application/json");

//    // Return a simple HTML document
//    //response.write("<html><body><b>TABLES</b></body></html>", true);

//    qDebug() << path;

//    if (path.empty())
//        response.write(m_dataLoader->data(), true);
//    else {
//        //response.write("<html><body><b>TABLE 1</b></body></html>", true);
//        processTableName(request, response, path);
//    }

//    qDebug() << response.getHeaders();



    response.setHeader("Content-Type", "application/json");
    qDebug() << pathList;

    //response.write(m_dataLoader->data(), true);

    if (request.getMethod() == "GET") {
        QByteArray resp = m_dataLoader->get(pathList);
        qDebug() << resp;
        if (!resp.isEmpty())
            response.write(resp, true);
        else
            response.setStatus(400, QString("Can't find %1").arg(QString(pathList.join('/'))).toLocal8Bit());
            response.write("<html><body><b>FUCK/b></body></html>");
    } else if (request.getMethod() == "POST")
        m_dataLoader->set(pathList, request.getBody());





}

void JsonTableController::processTableName(HttpRequest &request, HttpResponse &response, QByteArrayList &path)
{
    QByteArray tableName = path.takeFirst();

    //qDebug() << tableData;
    if (m_dataLoader->containsTable(tableName)) {
        if (path.empty()) {
            QByteArray tableData = m_dataLoader->table(tableName);
            response.write(tableData, true);
        } else {
            processTableValue(request, response, tableName, path);
        }
    } else {
        response.write(QString("<html><body><b>NO TABLE %1</b></body></html>").arg(QString(tableName)).toUtf8(), true);
    }
}

void JsonTableController::processTableValue(HttpRequest &request, HttpResponse &response, QByteArray tableName, QByteArrayList &path)
{
    QByteArray valueName = path.takeFirst();

    //qDebug() << tableData;
    if (m_dataLoader->containsValue(tableName, valueName)) {
        if (path.empty()) {
            QByteArray tableData = m_dataLoader->value(tableName, valueName);
            response.write(tableData, true);
        } else {
            processTableValue(request, response, tableName, path);
        }
    } else {
        response.write(QString("<html><body><b>NO VALUE %1 IN TABLE %2</b></body></html>").arg(QString(valueName), QString(tableName)).toUtf8(), true);
    }
}
