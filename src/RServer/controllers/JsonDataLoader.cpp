#include "JsonDataLoader.h"

#include <QFile>
#include <QTextStream>
#include <QJsonObject>
#include <QJsonArray>

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
    //QByteArray fileData = file.readAll();
    QTextStream textSteam(&file);
    //textSteam.setCodec("1251");
    QString fileData = textSteam.readAll();
    file.close();

    //qDebug() << fileData;

    QJsonParseError err;

    //qDebug() << fileData;
    m_jsonDoc = QJsonDocument::fromJson(fileData.toUtf8(), &err);

    //qDebug() << m_jsonDoc.toVariant() << err.errorString() << err.error;

    ok &= !m_jsonDoc.isEmpty();

    return ok;
}

QByteArray JsonDataLoader::data() const
{
    //int size;
    return m_jsonDoc.toJson();
}

QByteArray JsonDataLoader::table(QByteArray tableName)
{
    qDebug() << "NAME TABLE =" << tableName;

    foreach (auto tableEl, m_jsonDoc.object()["tables"].toArray()) {
        qDebug() << tableEl.toObject().value("id") << tableName << (tableEl.toObject().value("id") == tableName.toInt());

        if (tableEl.toObject().value("id") == tableName.toInt()) {
            qDebug() << "RET!" << tableEl;

            return QJsonDocument(tableEl.toObject()).toJson();
        }
    }
    //qDebug() << m_jsonDoc.object()["tables"].toArray()[0].toObject().value(name);

    return QByteArray();
}

QByteArray JsonDataLoader::value(QByteArray tableName, QByteArray valueName)
{
    //qDebug() << "\n" << m_jsonDoc.object()["tables"].toArray();

    foreach (auto tableEl, m_jsonDoc.object()["tables"].toArray()) {
        QJsonObject obj = tableEl.toObject();
        if (obj.value("id") == tableName.toInt()) {
            qDebug() << tableEl;

            if (obj.contains(valueName)) {
                if (obj[valueName].isArray())
                    return QJsonDocument(obj[valueName].toArray()).toJson();

                if (obj[valueName].isObject())
                    return QJsonDocument(obj[valueName].toObject()).toJson();

                if (obj[valueName].isDouble())
                    return QString::number(obj[valueName].isDouble()).toUtf8();
            }
        }
    }

    return QByteArray();
}

bool JsonDataLoader::containsTable(QByteArray tableName)
{
    foreach (auto tableEl, m_jsonDoc.object()["tables"].toArray()) {
        if (tableEl.toObject().value("id") == tableName.toInt()) {
            return true;
        }
    }

    return false;
}

bool JsonDataLoader::containsValue(QByteArray tableName, QByteArray valueName)
{
    foreach (auto tableEl, m_jsonDoc.object()["tables"].toArray()) {
        QJsonObject obj = tableEl.toObject();
        if (obj.value("id") == tableName.toInt()) {
            qDebug() << tableEl;

            if (obj.contains(valueName))
                return true;
        }
    }

    return false;
}

QByteArray JsonDataLoader::get(const QByteArrayList &pathList)
{
    //pathList[]
    //findChildren()
    //qDebug() << pathList << m_jsonDoc.object().value(pathList.at(0));
    QJsonArray mainObj = m_jsonDoc.object().value(pathList.at(0)).toArray();
    if (mainObj.isEmpty())
        return QByteArray();

//    foreach (auto tableEl, m_jsonDoc.object()["tables"].toArray()) {
//        QJsonObject obj = tableEl.toObject();
//        if (obj.value("id") == tableName.toInt()) {
//            qDebug() << tableEl;

//            if (obj.contains(valueName))
//                return true;
//        }
//    }

    return QJsonDocument(mainObj).toJson();

    return QByteArray("dfsbgdf");
}

bool JsonDataLoader::set(const QByteArrayList &pathList, const QByteArray &value)
{
    return true;
}

