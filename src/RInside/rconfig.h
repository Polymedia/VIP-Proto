#ifndef RCONFIG_H
#define RCONFIG_H

#include <QtCore/QtGlobal>

#if defined(RINSIDE_LIBRARY)
#  define RINSIDE_EXPORT Q_DECL_EXPORT
#else
#  define RINSIDE_EXPORT Q_DECL_IMPORT
#endif

#endif //RCONFIG_H
