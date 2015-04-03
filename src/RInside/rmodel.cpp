#include "rmodel.h"

#define USE_RINTERNALS
#include <Rinternals.h>

#include <QDebug>

template<typename T>
T plainData(SEXP value, int offset)
{
    return *((T *)&DATAPTR(value)->align + offset);
}

QVariant plain(SEXP value, int offset)
{
    switch (TYPEOF(value))
    {
    case LGLSXP: return (bool)plainData<Rboolean>(value, offset);
    case INTSXP: return plainData<int>(value, offset);
    case REALSXP: return plainData<double>(value, offset);
    case CPLXSXP: return QVariant();//plainData<Rcomplex>(value, offset);
    case STRSXP: return QString(CHAR(STRING_ELT(value, offset)));
    //case VECSXP: return plain(plainData<SEXP>(value, offset));
    default: return QVariant();
    }
}

RModel::RModel(SEXP object, QObject *parent) :
    QAbstractTableModel(parent),
    m_object(object)
{
}

RModel::~RModel()
{   
}

int RModel::rowCount(const QModelIndex &/*parent*/) const
{
    if (!Rf_isFrame(m_object))
        return Rf_isVector(m_object) ? Rf_length(m_object) : 1;

    SEXP rows = Rf_getAttrib(m_object, R_RowNamesSymbol);
    return Rf_isVector(rows) ? Rf_length(rows) : 1;
}

int RModel::columnCount(const QModelIndex &/*parent*/) const
{
    if (!Rf_isFrame(m_object))
        return 1;

    SEXP columns = Rf_getAttrib(m_object, R_NamesSymbol);
    return Rf_isVector(columns) ? Rf_length(columns) : 1;
}

QVariant RModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    if (Rf_isVector(m_object)) {
        if (TYPEOF(m_object) == VECSXP) {
            SEXP row = plainData<SEXP>(m_object, index.column());
            return plain(row, index.row());
        } else
            return plain(m_object, index.row());
    }

    return QVariant();
}

QVariant RModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (!Rf_isFrame(m_object))
        return (orientation == Qt::Horizontal) ? QString("Value") : QString::number(section + 1);

    if (orientation == Qt::Horizontal) {
        SEXP columns = Rf_getAttrib(m_object, R_NamesSymbol);
        return plain(columns, section);
    }
    if (orientation == Qt::Vertical) {
        SEXP rows = Rf_getAttrib(m_object, R_RowNamesSymbol);
        return plain(rows, section);
    }

    return QVariant();
}

