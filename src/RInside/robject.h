#ifndef ROBJECT_H
#define ROBJECT_H

#include "rconfig.h"
#include "shield.h"

#include <QString>
#include <QVector>
#include <QVariant>
#include <QMap>

typedef struct SEXPREC *SEXP;
class QAbstractTableModel;
class RModel;

class RINSIDE_EXPORT RObject
{
public:
    enum RStorage {
        Frame,
        Matrix,
        List,
        Array,
        Vector,
        Other
    };

    enum RType {
        Null,
        Bool,
        Int,
        Float,
        Point,
        String,
        Byte,
        Generic,
        Unknown
    };

public:
    RObject();
    RObject(SEXP s);
    ~RObject();

    RType type() const;
    RStorage storage() const;
    int length() const;
    int rows() const;
    int columns() const;

    RObject attrib(const QString &name) const;
    RObject data(int offset) const;
    QVariant value(int offset = 0) const;
    void setValue(QVariant val, int offset = 0);

    operator SEXP() const;

    static RObject fromModel(QAbstractTableModel *model);
    static RObject fromVariant(QVariant v);

private:
    RStorage evalStorage(SEXP object);
    RType evalType(SEXP object);

private:
    friend class RConsole;
    static QMap<QString, SEXP> m_shortcuts;

    SEXP m_object;
    RStorage m_storage;
    RType m_type;

};

#endif // ROBJECT_H
