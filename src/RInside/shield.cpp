#include "shield.h"

#define USE_RINTERNALS
#include <Rinternals.h>

Shield::Shield(SEXP s) :
m_object(Rf_protect(s))
{
}

Shield::~Shield()
{
    Rf_unprotect(1);
}
