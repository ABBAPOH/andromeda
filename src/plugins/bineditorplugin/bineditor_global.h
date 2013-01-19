#ifndef BINEDITOR_GLOBAL_H
#define BINEDITOR_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(HELLOWORLD_LIBRARY)
#  define BINEDITOR_EXPORT Q_DECL_EXPORT
#else
#  define BINEDITOR_EXPORT Q_DECL_IMPORT
#endif

#endif // BINEDITOR_GLOBAL_H
