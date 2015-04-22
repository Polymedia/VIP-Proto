#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

#include <httpserver/httprequesthandler.h>

class JsonTableController;

class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    RequestHandler(QObject *parent = 0);
    ~RequestHandler();

    void service(HttpRequest &request, HttpResponse &response);

signals:

public slots:

private:
    JsonTableController *m_jsonTableController;
};

#endif // REQUESTHANDLER_H
