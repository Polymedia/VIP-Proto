#include "RequestHandler.h"

#include "controllers/JsonTableController.h"

RequestHandler::RequestHandler(QObject *parent) :
    HttpRequestHandler(parent),
    m_jsonTableController(new JsonTableController)
{
    m_jsonTableController->loadData();
}

RequestHandler::~RequestHandler()
{
    delete m_jsonTableController;
}

void RequestHandler::service(HttpRequest &request, HttpResponse &response)
{
//    qDebug() << "getMethod" << request.getMethod();
//    qDebug() << "getPath" << request.getPath();
//    qDebug() << "getBody" << request.getBody();
//    qDebug() << "getParameterMap" << request.getParameterMap();

    QByteArray path = request.getPath();
    QByteArrayList pathList = path.split('/');
    if (pathList.last().isEmpty())
        pathList.removeLast();

    pathList.removeFirst();

    // Первый параметр отвечает за версию протокола
    if (pathList.length() > 1) {
        // TODO: Сделать абстрактный класс
        m_jsonTableController->setPathList(pathList);
        m_jsonTableController->service(request, response);
    } else {
        HttpRequestHandler::service(request, response);
    }
}
