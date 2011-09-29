#ifndef FILEIMAGEPROVIDER_H
#define FILEIMAGEPROVIDER_H

#include "io_global.h"

#include <QtGui/QFileIconProvider>

class IO_EXPORT FileImageProvider : public QFileIconProvider
{
public:
    FileImageProvider();

    virtual QImage image(const QFileInfo & info, const QSize size = QSize(128, 128)) const;

};

#endif // FILEIMAGEPROVIDER_H
