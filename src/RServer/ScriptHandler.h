#ifndef SCRIPTHANDLER_H
#define SCRIPTHANDLER_H

#include <QObject>

#define INPUT_FROM_CSV_NAME "inputFromCSV"
#define OPERATOR_OBJECT_NAME "operatorObject"
#define OUTPUT1_NAME "firstWidget"

class RConsole;
class RObject;

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

    QByteArray getOutputLikeJson();

    bool runScript(const QString &scriptName = "");

private:
    void fillInputRObject();

    QJsonObject prepareOutput1();

private:
    RConsole &m_rconsole;
    OperatorRObject m_inputRObject;
};

#endif // SCRIPTHANDLER_H
