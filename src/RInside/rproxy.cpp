#include "rproxy.h"
#include "rmodel.h"

#define USE_RINTERNALS
#include <Rinternals.h>

#include <QDebug>

bool RProxy::valid() const
{
    return m_object != SEXP();
}

int RProxy::type() const
{
    return TYPEOF(m_object);
}

RModel *RProxy::model()
{
    return new RModel(m_object);
}

RProxy::operator qint32() const
{
    return *INTEGER(m_object);
}

RProxy::operator qreal() const
{
    return *REAL(m_object);
}

RProxy::operator QString() const
{
    return CHAR(m_object);
}

RProxy::operator QVector<qint32>() const
{
    int size = Rf_length(m_object);
    SEXPREC_ALIGN *ptr = DATAPTR(m_object);

    QVector<qint32> result;
    result.resize(size);

    qint32 *begin = (qint32 *)&ptr->align;
    memcpy((char *)result.data(), begin, size * sizeof(qint32));

    return result;
}

RProxy::operator QVector<qreal>() const
{
    int size = Rf_length(m_object);
    SEXPREC_ALIGN *ptr = DATAPTR(m_object);

    QVector<qreal> result;
    result.resize(size);

    qreal *begin = (qreal *)&ptr->align;
    memcpy((char *)result.data(), begin, size * sizeof(qreal));

    return result;
}

RProxy::operator QVector<QString>() const
{
    int size = Rf_length(m_object);
    QVector<QString> result;
    result.resize(size);

    for (int i = 0; i < size; ++i)
        result[i] = CHAR(STRING_ELT(m_object, i));

    return result;
}

RProxy::operator DataFrame() const
{
    SEXP classVal = Rf_getAttrib(m_object, R_ClassSymbol);
    QString className = CHAR(STRING_ELT(classVal, 0));
    if (className != "data.frame") {
        qDebug() << "Not a data frame!" << className;
        return DataFrame();
    }

    //SEXP namesVal = Rf_getAttrib(m_object, R_NamesSymbol);
    //SEXP rowsVal = Rf_getAttrib(m_object, R_RowNamesSymbol);

    //TODO: handle data frame
    return DataFrame();
}
