#ifndef RPROXY_H
#define RPROXY_H

#include "rconfig.h"
#include "rmodel.h"

#include <QtGlobal>
#include <QString>
#include <QVector>
#include <QVariant>

typedef struct SEXPREC *SEXP;
class RModel;

struct RINSIDE_EXPORT DataFrame
{
    QVector<QVariant> names;
    QVector<QVariant> rowNames;
    QVector<QVariant> values;
};

class RINSIDE_EXPORT RProxy
{
public:
    RProxy() {}
    RProxy(SEXP s) : m_object(s) {}
    ~RProxy() {}

    bool valid() const;
    int type() const;
    SEXP data() const { return m_object; }

    //You should manuall destroy model
    RModel *model();

    operator qint32() const;
    operator qreal() const;
    operator QString() const;
    operator QVector<qint32>() const;
    operator QVector<qreal>() const;
    operator QVector<QString>() const;
    operator DataFrame() const;

private:
    SEXP m_object;
};

#endif // RPROXY_H

