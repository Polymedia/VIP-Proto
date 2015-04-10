#ifndef CSVMODEL_H
#define CSVMODEL_H

#include "rconfig.h"

#include <QAbstractTableModel>

class RINSIDE_EXPORT CsvModel : public QAbstractTableModel
{
public:
    CsvModel(QObject *parent = 0);
    ~CsvModel();

    //delim - разделяющий символ
    //headers - интерпретировать первую строку как названи¤ столбцов
    bool load(const QString &filename, const QChar &delim, bool headers = false);

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    QVector<QVariant> m_columnNames;
    QVector<QVariant> m_data;
    int m_rowCount;
    int m_columnCount;

};

#endif // CSVMODEL_H
