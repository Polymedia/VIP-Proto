#include "RequestHandler.h"

#include "controllers/JsonTableController.h"

RequestHandler::RequestHandler(QObject *parent) : HttpRequestHandler(parent)
{
}

RequestHandler::~RequestHandler()
{
}

void RequestHandler::service(HttpRequest &request, HttpResponse &response)
{
//    QByteArray path = request.getPath();
//    QByteArrayList pathList = path.split('/');
//    if (pathList.last().isEmpty())
//        pathList.removeLast();

//    qDebug("Conroller: path=%s", path.data());

//    if (pathList.at(1) == "tables") {
////        // Set a response header
////        response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

////        // Return a simple HTML document
////        response.write("<html><body><b>TABLES</b></body></html>", true);
//        qDebug() << "pathList 1" << pathList;
//        pathList.removeFirst();
//        pathList.removeFirst();
//        qDebug() << "pathList 2" << pathList;

//        JsonTableController().service(request, response, pathList);
//    } else {
//        HttpRequestHandler::service(request, response);
//    }


    QByteArray path = request.getPath();
    QByteArrayList pathList = path.split('/');
    if (pathList.last().isEmpty())
        pathList.removeLast();

    pathList.removeFirst();

    if (pathList.at(0) == "tables") {
        JsonTableController().service(request, response, pathList);
    } else {
        HttpRequestHandler::service(request, response);
    }
}
