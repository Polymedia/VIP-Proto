#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>

#define INPUT_FROM_CSV_NAME "inputFromCSV"
#define INPUT_FROM_DASHBOARD1_NAME "inputFromDashboard1"
#define OUTPUT1_NAME "firstWidget"

class RConsole;
class RObject;
class JsonTableController;
class HttpRequest;
class HttpResponse;

struct OperatorRObject {
    RObject *robj;
    QString name;
};

class ScriptHandler: public QObject
{
    Q_OBJECT
public:
    ScriptHandler(RConsole *r, QObject *parent = 0);
    ~ScriptHandler();

    void loadDataFromCSV(const QString &fileName = "KAZ_data-1.csv");
    bool loadDataFromJson(const QByteArray &jsonData);

    // эта функция удалится после приделки, скрипта
    void getResponse(HttpRequest &request, HttpResponse &response);

    QByteArray getOutputLikeJson();

    bool runScript(const QString &scriptName = "first_ex2.R");

private:
    void fillInputRObject();

    QJsonObject prepareOutput1();

private:
    RConsole &m_rconsole;
    OperatorRObject m_inputRObject;

    JsonTableController *m_jsonTableController;
};

#endif // SCRIPTHANDLER_H
