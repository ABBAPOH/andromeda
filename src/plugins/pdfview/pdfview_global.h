#ifndef PDFVIEW_GLOBAL_H
#define PDFVIEW_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PDFVIEW_LIBRARY)
#  define PDFVIEW_EXPORT Q_DECL_EXPORT
#else
#  define PDFVIEW_EXPORT Q_DECL_IMPORT
#endif

#endif // PDFVIEW_GLOBAL_H
