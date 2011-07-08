#ifndef IO_GLOBAL_H
#define IO_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IO_LIBRARY)
#  define IO_EXPORT Q_DECL_EXPORT
#else
#  define IO_EXPORT Q_DECL_IMPORT
#endif

#endif // IO_GLOBAL_H
