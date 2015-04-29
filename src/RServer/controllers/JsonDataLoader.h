#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>
#include <QJsonDocument>

class JsonDataLoader : public QObject
{
    Q_OBJECT
public:
    JsonDataLoader(QObject *parent = 0);
    ~JsonDataLoader();

    bool loadData(const QString &fileName = "dataBase.txt");

    QByteArray get(const QByteArrayList &pathList = QByteArrayList());
    bool set(const QByteArrayList &pathList, const QByteArray &value, QString &errorString = QString());

signals:

public slots:

private:
    QJsonDocument m_jsonDoc;
};

#endif // DATALOADER_H
