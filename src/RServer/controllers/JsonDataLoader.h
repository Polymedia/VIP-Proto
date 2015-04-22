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

    bool loadData(const QString &fileName);

    QByteArray get(const QByteArrayList &pathList);
    bool set(const QByteArrayList &pathList, const QByteArray &value);

signals:

public slots:

private:
    QJsonDocument m_jsonDoc;
};

#endif // DATALOADER_H
