#ifndef JSONTABLECONTROLLER_H
#define JSONTABLECONTROLLER_H

#include <QObject>

#include <httpserver/httprequesthandler.h>

#include "JsonDataLoader.h"

class JsonTableController : public HttpRequestHandler {
    Q_OBJECT
    //Q_DISABLE_COPY(JsonTableController)
public:

    /** Constructor */
    JsonTableController(QObject *parent = 0);
    ~JsonTableController();

    /** Generates the response */
    void service(HttpRequest &request, HttpResponse &response);

    void service(HttpRequest &request, HttpResponse &response, QByteArrayList path);

private:
    void processTableName(HttpRequest &request, HttpResponse &response, QByteArrayList &path);
    void processTableValue(HttpRequest &request, HttpResponse &response, QByteArray tableName, QByteArrayList &path);

private:
    JsonDataLoader *m_dataLoader;
};

#endif // JSONTABLECONTROLLER_H
