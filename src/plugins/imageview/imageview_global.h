#ifndef IMAGEVIEW_GLOBAL_H
#define IMAGEVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IMAGEVIEW_LIBRARY)
#  define IMAGEVIEW_EXPORT Q_DECL_EXPORT
#else
#  define IMAGEVIEW_EXPORT Q_DECL_IMPORT
#endif

#endif // IMAGEVIEW_GLOBAL_H
