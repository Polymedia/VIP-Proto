#include "rproxy.h"

#include <Rinternals.h>

int RProxy::type() const
{
    return TYPEOF(m_object);
}

RProxy::operator int() const
{
    return *INTEGER(m_object);
}


RProxy::operator double() const
{
    return *REAL(m_object);
}
