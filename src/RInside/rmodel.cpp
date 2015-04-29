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
        return m_object.attribute("row.names").length();
    case RObject::Matrix:
        return m_object.rows();
    case RObject::List: {
        int rows = 0;
        for (int i = 0; i < m_object.length(); ++i)
            rows = qMax(rows, m_object.data(i).length());
        return rows;
    }
    case RObject::Array: {
        RObject dims = m_object.attribute("dim");
        return dims.value(0).toInt() * dims.value(2).toInt();
    }
    case RObject::Factor:
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
        return m_object.attribute("names").length();
    case RObject::Matrix:
        return m_object.columns();
    case RObject::List:
        return m_object.length();
    case RObject::Array: {
        RObject dims = m_object.attribute("dim");
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
        RObject column = m_object.data(index.column());
        if (column.storage() == RObject::Factor) {
            int factor = column.value(index.row()).toInt();
            return column.attribute("levels").value(factor - 1);
        } else
            return column.value(index.row());
    }
    case RObject::Matrix: {
        int offset = index.row() * columnCount(index) + index.column();
        return m_object.value(offset);
    }
    case RObject::List: {
        RObject column = m_object.data(index.column());
        if (index.row() >= column.length())
            return QVariant("NaN");
        return column.value(index.row());
    }
    case RObject::Array: {
        int offset = index.row() * columnCount(index) + index.column();
        return m_object.value(offset);
    }
    case RObject::Factor: {
        int factor = m_object.value(index.row()).toInt();
        RObject levels = m_object.attribute("levels");

        if (factor - 1 > levels.length() || factor - 1 < 0)
            return "NaN";
        
        return levels.value(factor - 1);
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
        RObject columns = m_object.attribute(o ? "names" : "row.names");
        return columns.value(section);
    }

    if (m_object.storage() == RObject::List) {
        if (orientation == Qt::Horizontal) {
            RObject columns = m_object.attribute("names");
            return columns.value(section);
        }
    }

    return QString::number(section + 1);
}

Qt::ItemFlags RModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;

    if (m_object.storage() == RObject::List) {
        RObject column = m_object.data(index.column());
        if (index.row() < column.length())
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
        RObject column = m_object.data(index.column());
        if (column.storage() == RObject::Factor) {
            int factor = column.value(index.row()).toInt();
            column.attribute("levels").setValue(value, factor - 1);
        } else
            column.setValue(value, index.row());
        break;
    }
    case RObject::Matrix: {
        int offset = index.row() * columnCount(index) + index.column();
        m_object.setValue(value, offset);
        break;
    }
    case RObject::List: {
        RObject column = m_object.data(index.column());
        column.setValue(value, index.row());
        break;
    }
    case RObject::Array: {
        int offset = index.row() * columnCount(index) + index.column();
        m_object.setValue(value, offset);
        break;
    }
    case RObject::Factor: {
        int factor = m_object.value(index.row()).toInt();
        m_object.attribute("levels").setValue(value, factor - 1);
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
