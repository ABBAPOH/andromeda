#ifndef BOOKMARKS_GLOBAL_H
#define BOOKMARKS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(BOOKMARKS_LIBRARY)
#  define BOOKMARKS_EXPORT Q_DECL_EXPORT
#else
#  define BOOKMARKS_EXPORT Q_DECL_IMPORT
#endif

#endif // BOOKMARKS_GLOBAL_H
