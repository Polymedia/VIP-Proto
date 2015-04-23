#ifndef RMODEL_H
#define RMODEL_H

#include "rconfig.h"
#include "robject.h"

#include <QAbstractTableModel>

typedef struct SEXPREC *SEXP;

class RINSIDE_EXPORT RModel : public QAbstractTableModel
{
public:
    RModel(const RObject &object, QObject *parent = 0);
    ~RModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    
    //For editable model
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

private:
    RObject m_object;

};

#endif // RMODEL_H
