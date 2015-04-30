#ifndef JSONTABLECONTROLLER_H
#define JSONTABLECONTROLLER_H

#include <QObject>

#include <httpserver/httprequesthandler.h>

#include "JsonDataLoader.h"

class JsonTableController : public HttpRequestHandler {
    Q_OBJECT
public:

    /** Constructor */
    JsonTableController(QObject *parent = 0);
    ~JsonTableController();

    /** Generates the response */
    void service(HttpRequest &request, HttpResponse &response);

    bool loadData();

    void setPathList(const QByteArrayList &pathList);

private:
    JsonDataLoader *m_dataLoader;
    QByteArray m_protocolVersion;

    QByteArrayList m_pathList;
};

#endif // JSONTABLECONTROLLER_H
