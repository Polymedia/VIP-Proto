#ifndef RPROXY_H
#define RPROXY_H

typedef struct SEXPREC *SEXP;

#include "rconfig.h"

class RINSIDE_EXPORT RProxy
{
public:
    RProxy() {}
    RProxy(SEXP s) : m_object(s) {}
    ~RProxy() {}

    int type() const;

    template<typename T>
    operator T() { return T(); }

    operator int() const;
    operator double() const;

private:
    SEXP m_object;
};

#endif // RPROXY_H

