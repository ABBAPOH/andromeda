#ifndef GUISYSTEM_GLOBAL_H
#define GUISYSTEM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GUISYSTEM_LIBRARY)
#  define GUISYSTEM_EXPORT Q_DECL_EXPORT
#else
#  define GUISYSTEM_EXPORT Q_DECL_IMPORT
#endif

#endif // GUISYSTEM_GLOBAL_H
