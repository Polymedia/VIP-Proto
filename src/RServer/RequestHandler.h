#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

#include <httpserver/httprequesthandler.h>

class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    RequestHandler(QObject *parent = 0);
    ~RequestHandler();

    void service(HttpRequest &request, HttpResponse &response);

signals:

public slots:
};

#endif // REQUESTHANDLER_H
