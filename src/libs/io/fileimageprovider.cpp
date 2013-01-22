#include "fileimageprovider.h"
#ifdef Q_OS_MAC
#include <QuickLook/QuickLook.h>
#else
#endif

#include <QReadLocker>

FileImageProvider::FileImageProvider() :
    QFileIconProvider()
{
    m_lastCleanup.start();
}

QIcon FileImageProvider::icon(const QFileInfo &info) const
{
    QString path = info.absoluteFilePath();
    QIcon icon;
    int elapsed = 0;
    {
        QReadLocker l(&m_lock);
        elapsed = m_lastCleanup.elapsed();
        icon = m_iconMap.value(path);
    }

    if (elapsed > 10*1000) { // clean up every 10 seconds
        QWriteLocker l(&m_lock);
        m_lastCleanup.restart();
        QMutableMapIterator<QString, QIcon> it(m_iconMap);
        while (it.hasNext()) {
            if (it.next().value().isDetached()) {
                it.remove();
            }
        }
    }

    if (icon.isNull()) {
        icon = QFileIconProvider::icon(info);
        QWriteLocker l(&m_lock);
        m_iconMap.insert(path, icon);
    }
    return icon;
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
