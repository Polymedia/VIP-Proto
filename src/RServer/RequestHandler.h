#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

#include <httpserver/httprequesthandler.h>

class ScriptHandler;
class RConsole;

class RequestHandler : public HttpRequestHandler
{
    Q_OBJECT
public:
    RequestHandler(RConsole *r, QObject *parent = 0);
    ~RequestHandler();

    void service(HttpRequest &request, HttpResponse &response);

signals:

public slots:

private:
    ScriptHandler *m_scriptHandler;
};

#endif // REQUESTHANDLER_H
