#ifndef UNITTESTUTIL_H
#define UNITTESTUTIL_H

#include <QString>
#include <QFile>

class UnitTestUtil
{
public:
    static QByteArray readFileContents(const QString &path)
    {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
            return QByteArray();

        return file.readAll();
    }
};

#endif // UNITTESTUTIL_H
