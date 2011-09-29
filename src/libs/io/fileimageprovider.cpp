#include "fileimageprovider.h"
#ifdef Q_OS_MAC
#include <QuickLook/QuickLook.h>
#else
#endif

FileImageProvider::FileImageProvider() :
    QFileIconProvider()
{
}

QImage FileImageProvider::image(const QFileInfo &info, const QSize size) const
{
    QImage result;
#ifdef Q_OS_MAC
    QString path = info.absoluteFilePath();
    CGSize s;
    s.width = size.width();
    s.height = size.height();
    CFURLRef url = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault,
                                                                (const UInt8*)path.toUtf8().data(),
                                                                path.length(),
                                                                true);
    if (url) {
        CGImageRef image = QLThumbnailImageCreate(kCFAllocatorDefault, url, s, 0);
        if (image) {
            result = QPixmap::fromMacCGImageRef(image).toImage();
            CFRelease(image);
        }

        CFRelease(url);
    }
#else
#endif
    if (result.isNull())
        result = icon(info).pixmap(size).toImage();

    return result;
}
