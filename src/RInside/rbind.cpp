#include "rbind.h"

#include <Rinternals.h>

RBind::RBind(const QString &name) :
    m_name(name)
{
}

RBind& RBind::operator=(RObject object)
{
    SEXP nameSym = Rf_install(m_name.toLocal8Bit().constData());
    Rf_defineVar(nameSym, object, R_GlobalEnv);
    return *this;
}

RBind::operator RObject()
{
    SEXP nameSym = Rf_install(m_name.toLocal8Bit().constData());
    SEXP res = Rf_findVarInFrame(R_GlobalEnv, nameSym);

    if (res == R_UnboundValue)
        return R_NilValue;

    /* We need to evaluate if it is a promise */
    if (TYPEOF(res) == PROMSXP)
        res = Rf_eval(res, R_GlobalEnv);

    return res;
}