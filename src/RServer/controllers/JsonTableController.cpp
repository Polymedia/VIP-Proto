#include "JsonTableController.h"

JsonTableController::JsonTableController(QObject *parent) :
    HttpRequestHandler(parent),
    m_dataLoader(new JsonDataLoader)
{
}

JsonTableController::~JsonTableController()
{
    delete m_dataLoader;
}

void JsonTableController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "application/json");

    QByteArray returnErrorString = QString("Can't find %1").arg(QString(m_pathList.join('/'))).toLocal8Bit();

    if (request.getMethod() == "GET") {
        QByteArray resp = m_dataLoader->get(m_pathList);
        if (!resp.isEmpty())
            response.write(resp, true);
        else {
            response.setStatus(400, returnErrorString);
            response.write(returnErrorString, true);
        }
    } else if (request.getMethod() == "POST") {
        QString error;
        bool ok = m_dataLoader->set(m_pathList, request.getBody(), error);

        if (ok) {
            QByteArray newData = m_dataLoader->get();
            if (!newData.isEmpty())
                response.write(newData, true);
            else {
                response.setStatus(400, "Main array is empty");
                response.write("ERROR! Main array is empty", true);
            }
        } else {
            response.setStatus(400, error.toLocal8Bit());
            response.write(error.toLocal8Bit());
            response.write("\nSet Failed", true);
        }
    }
}

bool JsonTableController::loadData()
{
    bool ok = m_dataLoader->loadData();
    return ok;
}

void JsonTableController::setPathList(const QByteArrayList &pathList)
{
    QByteArrayList tmpPathList = pathList;
    m_protocolVersion = tmpPathList.takeAt(0);
    m_pathList = tmpPathList;
}
