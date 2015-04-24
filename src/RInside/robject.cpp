#include "robject.h"
#include "rmodel.h"
#include "shield.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#define USE_RINTERNALS
#include <Rinternals.h>

template<typename T>
static T& plainData(SEXP value, int offset)
{
    return ((T*)DATAPTR(value))[offset];
}

static QVariant plain(SEXP value, int offset)
{
    switch (TYPEOF(value))
    {
    case LGLSXP: return (bool)plainData<int>(value, offset);
    case INTSXP: return plainData<int>(value, offset);
    case REALSXP: return plainData<double>(value, offset);
    case CPLXSXP: {
        Rcomplex c = plainData<Rcomplex>(value, offset);
        return QPointF(c.r, c.i);
    }
    case STRSXP: {
        const char *ut = CHAR(plainData<SEXP>(value, offset));
        return QString::fromUtf8(ut);
    }
    case RAWSXP: return QByteArray(CHAR(plainData<SEXP>(value, offset)));
    default: return QVariant();
    }
}

static void setPlain(SEXP value, int offset, QVariant val)
{
    switch (TYPEOF(value))
    {
    case LGLSXP:
        plainData<int>(value, offset) = val.toInt();
        break;
    case INTSXP:
        plainData<int>(value, offset) = val.toInt();
        break;
    case REALSXP:
        plainData<double>(value, offset) = val.toDouble();
        break;
    case CPLXSXP: {
        QPointF pnt = val.toPointF();
        plainData<Rcomplex>(value, offset) = Rcomplex{ pnt.x(), pnt.y() };
        break;
    }
    case STRSXP: {
        QString qstr = val.toString();
        plainData<SEXP>(value, offset) = Rf_mkChar(qstr.toUtf8().constData());
        break;
    }
    case RAWSXP: {
        QByteArray qraw = val.toByteArray();
        plainData<SEXP>(value, offset) = Rf_mkChar(qraw.constData());
        break;
    }
    default:
        break;
    }
}

static int rType(int type)
{
    switch (type)
    {
    case QMetaType::Bool: return LGLSXP;
    case QMetaType::Int: return INTSXP;
    case QMetaType::Double: return REALSXP;
    case QMetaType::Float: return REALSXP;
    case QMetaType::QPointF: return CPLXSXP;
    case QMetaType::QString: return STRSXP;
    case QMetaType::QByteArray: return RAWSXP;
    default: return NILSXP;
    }
}

static int rType(RObject::RType type)
{
    switch (type)
    {
    case RObject::Null: return NILSXP;
    case RObject::Bool: return LGLSXP;
    case RObject::Int: return INTSXP;
    case RObject::Float: return REALSXP;
    case RObject::Point: return CPLXSXP;
    case RObject::String: return STRSXP;
    case RObject::Byte: return RAWSXP;
    case RObject::Generic: return VECSXP;
    default: return NILSXP;
    }
}

static SEXP createDataframe(int rows, int columns)
{
    Shield df(Rf_allocVector(VECSXP, columns));
    Rf_setAttrib(df, R_NamesSymbol, Rf_allocVector(STRSXP, columns));
    Rf_setAttrib(df, R_RowNamesSymbol, Rf_allocVector(STRSXP, rows));
    Rf_setAttrib(df, R_ClassSymbol, Rf_mkString("data.frame"));
    return df;
}

static SEXP createMatrix(RObject::RType type, int rows, int columns)
{
    return Rf_allocMatrix(rType(type), rows, columns);
}

static SEXP createList(int rows)
{
    return Rf_allocVector(VECSXP, rows);
}

static SEXP createArray(RObject::RType type, int rows, int columns)
{
    Shield dim(allocVector(INTSXP, 2));
    plainData<int>(dim, 0) = rows;
    plainData<int>(dim, 1) = columns;
    return Rf_allocArray(rType(type), dim);
}

static SEXP createVector(RObject::RType type, int rows)
{
    return Rf_allocVector(rType(type), rows);
}

QMap<QString, SEXP> RObject::m_shortcuts;

RObject::RObject(SEXP s) :
    m_object(s),
    m_storage(evalStorage(m_object)),
    m_type(evalType(m_object))
{
}

RObject::RObject(RStorage storage, RType type, int rows, int columns)
{
    switch (storage)
    {
    case RObject::Frame:
        m_object = createDataframe(rows, columns);
        for (int i = 0; i < columns; i++)
            plainData<SEXP>(m_object, i) = Rf_allocVector(rType(type), rows);
        break;
    case RObject::Matrix:
        m_object = createMatrix(type, rows, columns);
        break;
    case RObject::List:
        m_object = createList(rows);
        break;
    case RObject::Array:
        m_object = createArray(type, rows, columns);
        break;
    case RObject::Vector:
        m_object = createVector(type, rows);
        break;
    default:
        m_object = R_NilValue;
        break;
    }

    m_storage = evalStorage(m_object);
    m_type = evalType(m_object);
}

RObject::~RObject()
{
}

RObject::RType RObject::type() const
{
    return m_type;
}

RObject::RStorage RObject::storage() const
{
    return m_storage;
}

int RObject::length() const
{
    return Rf_length(m_object);
}

int RObject::rows() const
{
    return Rf_nrows(m_object);
}

int RObject::columns() const
{
    return Rf_ncols(m_object);
}

RObject RObject::attribute(const QString &name) const
{
    SEXP rname = m_shortcuts.value(name, R_NilValue);
    if (rname == SEXP() || rname == R_NilValue)
        rname = Rf_mkChar(name.toUtf8().constData());
    return Rf_getAttrib(m_object, rname);
}

void RObject::setAttribute(const QString &name, RObject val)
{
    Shield obj(m_object);
    SEXP rname = m_shortcuts.value(name, R_NilValue);
    if (rname == SEXP() || rname == R_NilValue)
        rname = Rf_mkChar(name.toUtf8().constData());
    Rf_setAttrib(obj, rname, val);
}

RObject RObject::data(int offset) const
{
    return plainData<SEXP>(m_object, offset);
}

void RObject::setData(RObject val, int offset)
{
    Shield obj(m_object);
    plainData<SEXP>(obj, offset) = val;
}

QVariant RObject::value(int offset) const
{
    return plain(m_object, offset);
}

void RObject::setValue(QVariant val, int offset)
{
    Shield obj(m_object);
    setPlain(obj, offset, val);
}

RObject::operator SEXP() const
{
    return m_object;
}

RObject RObject::fromModel(QAbstractTableModel *model)
{
    Q_ASSERT(model);

    int columns = model->columnCount();
    int rows = model->rowCount();

    //Create data frame
    Shield df(createDataframe(rows, columns));

    //Load column names
    SEXP names = Rf_getAttrib(df, R_NamesSymbol);
    for (int i = 0; i < columns; i++) {
        QString name = model->headerData(i, Qt::Horizontal).toString();
        plainData<SEXP>(names, i) = Rf_mkChar(name.toUtf8().constData());
        //Get type from first row
        QVariant v = model->data(model->index(0, i));
        plainData<SEXP>(df, i) = Rf_allocVector(rType(v.type()), rows);
    }

    //Load row numbers
    SEXP rowNames = Rf_getAttrib(df, R_RowNamesSymbol);
    for (int i = 0; i < rows; ++i) {
        int rowName = model->headerData(i, Qt::Vertical).toInt();
        setPlain(rowNames, i, rowName + 1);
    }

    //Load data
    for (int j = 0; j < columns; j++) {
        SEXP col = plainData<SEXP>(df, j);
        for (int i = 0; i < rows; ++i) {
            setPlain(col, i, model->data(model->index(i, j)));
        }
    }

    return RObject(df);
}

RObject RObject::fromVariant(QVariant v)
{
    switch (v.type())
    {
    case QMetaType::Bool:
        return Rf_ScalarLogical(v.toBool());
    case QMetaType::Int:
        return Rf_ScalarInteger(v.toInt());
    case QMetaType::Double:
    case QMetaType::Float:
        return Rf_ScalarReal(v.toDouble());
    case QMetaType::QString: {
        QString qstr = v.toString();
        SEXP str = Rf_mkChar(qstr.toUtf8().constData());
        return Rf_ScalarString(str);
    }
    default:
        return RObject();
    }
}

RObject RObject::fromJsonObject(const QByteArray &ba)
{
    /// Конвертим из JSON вида:
    ///     {
    ///         "headers"   : ["col 1", "col 2", ...],
    ///         "rows"      : ["row 1", "row 2", ...],
    ///         "values"    : [ [1, 2, 3, ...],
    ///                         [4, 5, 6, ...], ...
    ///                       ]
    ///     }
    /// количество элементов в "rows" должно совпадать с количеством "строк" в массиве values
    /// количество эелементов в "headers" должно совпадать с количеством "столбцов" в массиве values

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &err);

    if (err.error != QJsonParseError::NoError)
        return RObject();

    QJsonObject mainObj = jsonDoc.object();
    if (mainObj.isEmpty())
        return RObject();

    if (!mainObj.contains("rows") || !mainObj.contains("headers") || !mainObj.contains("values"))
        return RObject();

    QJsonArray rows, headers, values;
    rows = mainObj.value("rows").toArray();
    headers = mainObj.value("headers").toArray();
    values =  mainObj.value("values").toArray();

    int rowsCount, headersCount, valuesCount;


    rowsCount = rows.size();
    headersCount = headers.size();
    valuesCount = values.size();

    if (headersCount == 0 || rowsCount == 0 || valuesCount == 0 || valuesCount != rowsCount)
        return RObject();

    //Create data frame
    Shield df(createDataframe(rowsCount, headersCount));

    //Load column names
    SEXP headerNames = Rf_getAttrib(df, R_NamesSymbol);
    for (int i = 0; i < headersCount; i++) {
        QString headerName = headers.at(i).toString();
        plainData<SEXP>(headerNames, i) = Rf_mkChar(headerName.toUtf8().constData());
        // TODO!
        // пока предполагаем, что все данные - Float
        plainData<SEXP>(df, i) = Rf_allocVector(rType(RType::Float), rowsCount);
    }

    //Load row numbers
    SEXP rowNames = Rf_getAttrib(df, R_RowNamesSymbol);
    for (int i = 0; i < rowsCount; ++i) {
        QString rowName = rows.at(i).toString();
        setPlain(rowNames, i, rowName);
    }

    //Load data
    for (int i = 0; i < rowsCount; ++i) {
        QJsonArray row = values.at(i).toArray();
        if (row.isEmpty() || row.count() != headersCount)
            return RObject();

        for (int j = 0; j < headersCount; j++) {
            SEXP col = plainData<SEXP>(df, j);
            setPlain(col, i, row.at(j).toVariant());
        }
    }

    return RObject(df);
}

void RObject::fill(QVariant var)
{
    switch (m_storage) {
    case Frame:
        for (int j = 0; j < attribute("names").length(); j++) {
            SEXP col = plainData<SEXP>(m_object, j);
            for (int i = 0; i < attribute("row.names").length(); ++i) {
                setPlain(col, i, var);
            }
        }
        break;
    case Matrix:
        for (int j = 0; j < columns(); j++) {
            SEXP col = plainData<SEXP>(m_object, j);
            for (int i = 0; i < rows(); ++i) {
                setPlain(col, i, var);
            }
        }
        break;
    case List:
        for (int j = 0; j < length(); j++) {
            SEXP col = plainData<SEXP>(m_object, j);
            for (int i = 0; i < data(i).length(); ++i) {
                setPlain(col, i, var);
            }
        }
        break;
    case Array: {
        RObject dims = attribute("dim");
        for (int j = 0; j < dims.value(1).toInt(); j++) {
            SEXP col = plainData<SEXP>(m_object, j);
            for (int i = 0; i < dims.value(0).toInt() * dims.value(2).toInt(); ++i) {
                setPlain(col, i, var);
            }
        }
        break;
    }
    case Vector:
        for (int i = 0; i < length(); ++i) {
            SEXP col = plainData<SEXP>(m_object, i);
            setPlain(col, i, var);
        }
    case Other: break;
    }
}

RObject::RStorage RObject::evalStorage(SEXP object)
{
    if (Rf_isFrame(object))
        return Frame;

    if (Rf_isMatrix(object))
        return Matrix;

    if (Rf_isList(object) || Rf_isNewList(m_object))
        return List;

    if (Rf_isArray(m_object))
        return Array;

    if (Rf_isVector(m_object))
        return Vector;

    return Other;
}

RObject::RType RObject::evalType(SEXP object)
{
    switch (TYPEOF(object))
    {
    case NILSXP: return Null;
    case LGLSXP: return Bool;
    case INTSXP: return Int;
    case REALSXP: return Float;
    case CPLXSXP: return Point;
    case STRSXP: return String;
    case RAWSXP: return Byte;
    case VECSXP: return Generic;
    default: return Unknown;
    }
}
