#ifndef RMODEL_H
#define RMODEL_H

#include <QAbstractTableModel>

typedef struct SEXPREC *SEXP;

class RModel : public QAbstractTableModel
{
public:
    RModel(SEXP object, QObject *parent = 0);
    ~RModel();

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:
    SEXP m_object;

};

#endif // RMODEL_H
