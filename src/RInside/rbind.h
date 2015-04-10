#ifndef RBIND_H
#define RBIND_H

#include "rconfig.h"
#include "robject.h"
#include <QString>

class RINSIDE_EXPORT RBind
{
public:
    RBind(const QString &name);

    RBind& operator=(RObject object);
    operator RObject();

private:
    QString m_name;
};

#endif //RBIND_H