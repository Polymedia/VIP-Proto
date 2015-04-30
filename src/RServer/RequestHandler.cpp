#include "RequestHandler.h"

#include "ScriptHandler.h"

RequestHandler::RequestHandler(RConsole *r, QObject *parent) :
    HttpRequestHandler(parent)
{
    m_scriptHandler = new ScriptHandler(r, this);
}

RequestHandler::~RequestHandler()
{
    delete m_scriptHandler;
}

void RequestHandler::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path = request.getPath();
    QByteArrayList pathList = path.split('/');
    if (pathList.last().isEmpty())
        pathList.removeLast();

    pathList.removeFirst();

    // Первый параметр отвечает за версию протокола
    if (pathList.length() > 1) {
        // TODO: придумать "абстрактную" обработку запросов

        response.setHeader("Content-Type", "application/json; charset=utf-8");

        if (request.getMethod() == "POST") {
            bool ok = m_scriptHandler->loadDataFromJson(request.getBody());
            if (!ok) {
                response.setStatus(400, "Bad input");
                response.write("Bad input", true);
                return;
            }
        }

        bool ok = m_scriptHandler->runScript();
        if (!ok) {
            response.setStatus(500, "Script error");
            response.write("Script error", true);
            return;
        }

        QByteArray output = m_scriptHandler->getOutputLikeJson();

        response.write(output, true);
        return;
    } else {
        HttpRequestHandler::service(request, response);
    }
}
