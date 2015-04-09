#include "rmodel.h"

RModel::RModel(const RObject &object, QObject *parent) :
    QAbstractTableModel(parent),
    m_object(object)
{
}

RModel::~RModel()
{   
}

int RModel::rowCount(const QModelIndex &/*parent*/) const
{
    switch (m_object.storage())
    {
    case RObject::Frame:
        return m_object.attrib("row.names").length();
    case RObject::Matrix:
        return m_object.rows();
    case RObject::Array: {
        RObject dims = m_object.attrib("dim");
        return dims.value(0).toInt() * dims.value(2).toInt();
    }
    case RObject::Vector:
        return m_object.length();
    default:
        return 1;
    }
}

int RModel::columnCount(const QModelIndex &/*parent*/) const
{
    switch (m_object.storage())
    {
    case RObject::Frame:
        return m_object.attrib("names").length();
    case RObject::Matrix:
        return m_object.columns();
    case RObject::List: {
        int columns = 0;
        for (int i = 0; i < m_object.length(); ++i)
            columns = qMax(columns, m_object.data(i).length());
        return columns;
    }
    case RObject::Array: {
        RObject dims = m_object.attrib("dim");
        return dims.value(1).toInt();
    }
    default:
        return 1;
    }
}

QVariant RModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    switch (m_object.storage())
    {
    case RObject::Frame: {
        RObject row = m_object.data(index.column());
        return row.value(index.row());
    }
    case RObject::Matrix: {
        int offset = index.row() * columnCount(index) + index.column();
        return m_object.value(offset);
    }
    case RObject::List: {
        RObject row = m_object.data(index.column());
        if (index.column() >= row.length())
            return QVariant("NaN");
        return row.value(index.column());
    }
    case RObject::Array: {
        int offset = index.row() * columnCount(index) + index.column();
        return m_object.value(offset);
    }
    case RObject::Vector:
        return m_object.value(index.row());
    default:
        return QVariant();
    }
}

QVariant RModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (m_object.storage() == RObject::Frame) {
        bool o = orientation == Qt::Horizontal;
        RObject columns = m_object.attrib(o ? "names" : "row.names");
        return columns.value(section);
    }

    return QString::number(section + 1);
}

Qt::ItemFlags RModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (m_object.storage() == RObject::List) {
        RObject row = m_object.data(index.column());
        if (index.column() < row.length())
            flags |= Qt::ItemIsEditable;
    }
    else
        flags |= Qt::ItemIsEditable;

    return flags;
}

bool RModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    switch (m_object.storage())
    {
    case RObject::Frame: {
        RObject row = m_object.data(index.column());
        row.setValue(value, index.row());
        break;
    }
    case RObject::Matrix: {
        int offset = index.row() * columnCount(index) + index.column();
        m_object.setValue(value, offset);
        break;
    }
    case RObject::List: {
        RObject row = m_object.data(index.column());
        row.setValue(value, index.column());
        break;
    }
    case RObject::Array: {
        int offset = index.row() * columnCount(index) + index.column();
        m_object.setValue(value, offset);
        break;
    }
    case RObject::Vector:
        m_object.setValue(value, index.row());
        break;
    default:
        return false;
    }

    emit dataChanged(index, index);
    return true;
}
