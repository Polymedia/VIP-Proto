#ifndef RSHIELD_H
#define RSHIELD_H

#include "rconfig.h"

typedef struct SEXPREC *SEXP;

class RINSIDE_EXPORT Shield
{
public:
    Shield(SEXP s);
    ~Shield();

    operator SEXP() const { return m_object; }

private:
    SEXP m_object;

};

#endif