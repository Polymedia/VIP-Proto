#include "RequestHandler.h"

RequestHandler::RequestHandler(QObject *parent) : HttpRequestHandler(parent)
{
}

RequestHandler::~RequestHandler()
{
}

void RequestHandler::service(HttpRequest &request, HttpResponse &response) {
    QByteArray path = request.getPath();
    qDebug("Conroller: path=%s", path.data());

    // Set a response header
    response.setHeader("Content-Type", "text/html; charset=ISO-8859-1");

    // Return a simple HTML document
    response.write("<html><body><b>Hello World</b></body></html>", true);

    qDebug("Conroller: finished request");
    //logger->clear();
}
