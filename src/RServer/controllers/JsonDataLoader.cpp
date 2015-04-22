#include "JsonDataLoader.h"

#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDebug>

JsonDataLoader::JsonDataLoader(QObject *parent) : QObject(parent)
{
}

JsonDataLoader::~JsonDataLoader()
{
}

bool JsonDataLoader::loadData(const QString &fileName)
{
    bool ok = true;
    QFile file(fileName);
    ok &= file.open(QIODevice::ReadOnly);
    QTextStream textSteam(&file);
    QString fileData = textSteam.readAll();
    file.close();

    QJsonParseError err;
    m_jsonDoc = QJsonDocument::fromJson(fileData.toUtf8(), &err);

    if (err.error != QJsonParseError::NoError) {
        qDebug() << err.errorString() << err.error;
    }

    ok &= !m_jsonDoc.isEmpty();

    return ok;
}

QByteArray JsonDataLoader::get(const QByteArrayList &pathList)
{
    // TODO: придумать норм парсилку

    if (pathList.isEmpty())
        return QByteArray("");

    QJsonArray mainObj = m_jsonDoc.object().value(pathList.at(0)).toArray();
    if (mainObj.isEmpty())
        return QByteArray();

    if (pathList.length() == 1) {
        return QJsonDocument(mainObj).toJson();
    }

    if (pathList.length() == 2) {
        foreach (auto tableEl, mainObj) {
            QJsonObject obj = tableEl.toObject();
            if (!obj.isEmpty() && obj.value("id").toString() == pathList.at(1)) {
                return QJsonDocument(obj).toJson();
            }
        }

        return QByteArray("");
    }

    if (pathList.length() == 3) {
        foreach (auto tableEl, mainObj) {
            QJsonObject obj = tableEl.toObject();
            if (!obj.isEmpty() && obj.value("id").toString() == pathList.at(1)) {
                return QJsonDocument(obj.value(pathList.at(2)).toArray()).toJson();
            }
        }

        return QByteArray("");
    }

    return QByteArray("");
}

bool JsonDataLoader::set(const QByteArrayList &pathList, const QByteArray &value)
{
    // TODO: придумать норм парсилку

    if (pathList.isEmpty())
        return true;

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(value, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << err.errorString();

        return false;
    }

    QJsonObject mainObj = m_jsonDoc.object();
    QJsonArray mainArray = mainObj.value(pathList.at(0)).toArray();
    if (mainArray.isEmpty())
        return false;



    bool isCahnged = false;

    if (pathList.length() == 1) {
        if (!jsonDoc.isArray())
            return false;

        mainObj.insert("widgets", jsonDoc.array());
        m_jsonDoc = QJsonDocument(mainObj);

        return true;
    }

    for (int i = 0; i < mainArray.size(); i++) {
        if (pathList.length() > 1 && mainArray[i].toObject().value("id").toString() == pathList.at(1)) {
            if (pathList.length() == 2) {
                mainArray.takeAt(i);
                mainArray.insert(i, jsonDoc.object());
                isCahnged = true;
                break;
            }

            if (pathList.length() == 3) {
                QJsonObject newVal = mainArray.takeAt(i).toObject();
                if (jsonDoc.isObject())
                    newVal.insert(pathList.at(2), jsonDoc.object());
                else if (jsonDoc.isArray())
                    newVal.insert(pathList.at(2), jsonDoc.array());

                mainArray.insert(i, newVal);

                isCahnged = true;

                break;
            }
        }
    }

    if (isCahnged) {
        mainObj.insert(pathList.at(0), mainArray);
        m_jsonDoc = QJsonDocument(mainObj);

        return true;
    }
}

