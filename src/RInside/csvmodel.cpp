#include "csvmodel.h"

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QDebug>

static QMetaType::Type guessType(const QString &value)
{
    bool ok = false;
    value.toInt(&ok);
    if (ok)
        return QMetaType::Int;

    value.toFloat(&ok);
    if (ok)
        return QMetaType::Float;

    QRegExp exp("(true|false)");
    ok = exp.exactMatch(value);
    if (ok)
        return QMetaType::Bool;

    return QMetaType::QString;
}

CsvModel::CsvModel(QObject *parent) :
    QAbstractTableModel(parent),
    m_rowCount(0),
    m_columnCount(0)
{
}

CsvModel::~CsvModel()
{
}

bool CsvModel::load(QIODevice *device, const QChar &delim, bool headers)
{
    QTextStream in(device);

    //Get column names
    if (headers) {
        QString headerLine = in.readLine();
        QStringList headers = headerLine.split(delim);
        m_columnNames.resize(headers.size());
        for (int i = 0; i < headers.size(); ++i) {
            QString head = headers[i];
            head = head.simplified();
            m_columnNames[i] = head;
        }
    }

    //Read first line
    qint64 prevPos = in.pos();
    QString firstLine = in.readLine();
    QStringList values = firstLine.split(delim);
    m_columnCount = values.size();
    in.seek(prevPos);

    //Try to guess types
    QVector<QMetaType::Type> types(values.size());
    for (int i = 0; i < values.size(); ++i) {
        QString value = values[i];
        value = value.simplified();
        types[i] = guessType(value);
    }

    qint64 lineCount = headers ? 0 : 1;
    while (!in.atEnd()) {
        lineCount++;
        QString line = in.readLine();
        QStringList values = line.split(delim);
        for (int i = 0; i < values.size(); ++i) {
            QString value = values[i];
            value = value.simplified();
            QVariant data(value);

            //Check if value can be converted
            if (!data.convert(types[i])) {
                QString warn("Error (line %1): %2 cannot be converted to %3");
                warn = warn.arg(lineCount).arg(value).arg(QMetaType::typeName(types[i]));
                qWarning() << warn;
            }

            m_data.append(data);
        }
    }

    m_rowCount = m_data.size() / m_columnCount;

    return true;
}

int CsvModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_rowCount;
}

int CsvModel::columnCount(const QModelIndex &/*parent*/) const
{
    return m_columnCount;
}

QVariant CsvModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    int offset = index.row() * columnCount(index) + index.column();
    return m_data[offset];
}

QVariant CsvModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return m_columnNames[section];
    else
        return section;
}
