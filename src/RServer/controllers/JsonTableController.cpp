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
        bool ok = m_dataLoader->set(m_pathList, request.getBody());

        if (ok) {
            QByteArray newData = m_dataLoader->get(QByteArrayList() << m_pathList.at(0));
            if (!newData.isEmpty())
                response.write(newData, true);
            else {
                response.setStatus(400, returnErrorString);
                response.write(returnErrorString, true);
            }
        } else {
            response.write("Set Failed", true);
        }
    }
}

bool JsonTableController::loadData()
{
    bool ok = m_dataLoader->loadData("dataBase.txt");
    return ok;
}

void JsonTableController::setPathList(const QByteArrayList &pathList)
{
    QByteArrayList tmpPathList = pathList;
    m_protocolVersion = tmpPathList.takeAt(0);
    m_pathList = tmpPathList;
}
