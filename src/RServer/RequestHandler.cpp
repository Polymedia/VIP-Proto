#include "RequestHandler.h"

#include "controllers/JsonTableController.h"
#include "ScriptHandler.h"

RequestHandler::RequestHandler(RConsole *r, QObject *parent) :
    HttpRequestHandler(parent),
    m_jsonTableController(new JsonTableController)
{
    m_jsonTableController->loadData();
    m_scriptHandler = new ScriptHandler(r, this);
}

RequestHandler::~RequestHandler()
{
    delete m_jsonTableController;
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

        if (request.getMethod() == "GET") {
            // TODO:
            // здесь надо запускать реальный скрипт,
            // который возвращает JSON массив отупут переменных
            // сейчас заглушка, потом надо избавиться от JsonTableController
            m_jsonTableController->setPathList(pathList);
            m_jsonTableController->service(request, response);
            return;
        }

        if (request.getMethod() == "POST") {
            bool ok = m_scriptHandler->loadDataFromJson(request.getBody());

            if (!ok) {
                response.setStatus(400, "Bad input");
                response.write("Bad input", true);
                return;
            }

            ok = m_scriptHandler->runScript();

            if (!ok) {
                response.setStatus(500, "Script error");
                response.write("Script error", true);
                return;
            }

            QByteArray output = m_scriptHandler->getOutputLikeJson();
            response.write(output, true);
            return;
        }
    } else {
        HttpRequestHandler::service(request, response);
    }
}
