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
    case NILSXP: return "Null";
    case LGLSXP: return (bool)plainData<Rboolean>(value, offset);
    case INTSXP: return plainData<int>(value, offset);
    case REALSXP: return plainData<double>(value, offset);
    case CPLXSXP: {
        Rcomplex c = plainData<Rcomplex>(value, offset);
        return QPointF(c.r, c.i);
    }
    case STRSXP: return QString(CHAR(STRING_ELT(value, offset)));
    case RAWSXP: return "Bytes";
    case LISTSXP:
    case VECSXP: return "List";
    case LANGSXP: return "Expression";
    default: return "???";
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
    if (Rf_isFrame(m_object)) {
        SEXP rows = Rf_getAttrib(m_object, R_RowNamesSymbol);
        return Rf_length(rows);
    }

    if (Rf_isMatrix(m_object))
        return Rf_nrows(m_object);

    if (Rf_isArray(m_object)) {
        SEXP dims = Rf_getAttrib(m_object, R_DimSymbol);
        return plain(dims, 0).toInt() * plain(dims, 2).toInt();
    }

    if (Rf_isVector(m_object))
        return Rf_length(m_object);

    return 1;
}

int RModel::columnCount(const QModelIndex &/*parent*/) const
{
    if (Rf_isFrame(m_object)) {
        SEXP columns = Rf_getAttrib(m_object, R_NamesSymbol);
        return Rf_isVector(columns) ? Rf_length(columns) : 1;
    }

    if (Rf_isMatrix(m_object))
        return Rf_ncols(m_object);

    if (Rf_isList(m_object) || Rf_isNewList(m_object)) {
        int columns = 0;
        for (int i = 0; i < Rf_length(m_object); ++i)
            columns = qMax(columns, Rf_length(plainData<SEXP>(m_object, i)));
        return columns;
    }

    if (Rf_isArray(m_object)) {
        SEXP dims = Rf_getAttrib(m_object, R_DimSymbol);
        return plain(dims, 1).toInt();
    }

    return 1;
}

QVariant RModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    if (Rf_isFrame(m_object)) {
        SEXP row = plainData<SEXP>(m_object, index.column());
        return plain(row, index.row());
    }

    if (Rf_isMatrix(m_object)) {
        int offset = index.row() * columnCount(index) + index.column();
        return plain(m_object, offset);
    }

    if (Rf_isList(m_object) || Rf_isNewList(m_object)) {
        SEXP row = plainData<SEXP>(m_object, index.row());
        if (index.column() >= Rf_length(row))
            return QVariant("NaN");
        return plain(row, index.column());
    }

    if (Rf_isArray(m_object)) {
        int offset = index.row() * columnCount(index) + index.column();
        return plain(m_object, offset);
    }

    if (Rf_isVector(m_object))
        return plain(m_object, index.row());

    return QVariant();
}

QVariant RModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (Rf_isFrame(m_object)) {
        bool o = orientation == Qt::Horizontal;
        SEXP columns = Rf_getAttrib(m_object, o ? R_NamesSymbol : R_RowNamesSymbol);
        return plain(columns, section);
    }

    /*if (Rf_isArray(m_object)) {

    }*/

    return QString::number(section + 1);
}

