#include "filethumbnails.h"

#include <QThread>
#include <QImageReader>
#include <QPainter>

using namespace FileManager;

Worker::Worker(QObject *parent) :
    QObject(parent)
{
}

void Worker::getThumbnails(const QStringList &paths)
{
    foreach(const QString &path, paths) {
        emit done(path, getThumb(path));
    }
}

QImage Worker::getThumb(const QString &path)
{
    static const int thumbExtent = 128;
    static const int imageExtent = 125;

    QImage image, thumb(thumbExtent, thumbExtent, QImage::Format_ARGB32);
    QImageReader reader(path);
    int w = reader.size().width();
    int h = reader.size().height();

    if (w > h) {
        int newHeight = h * imageExtent / w;
        reader.setScaledSize(QSize(imageExtent, newHeight));
    } else {
        int newWidth = w * imageExtent / h;
        reader.setScaledSize(QSize(newWidth, imageExtent));
    }

    image = reader.read();

    QPainter painter(&thumb);
    painter.fillRect(QRect(0,0, thumbExtent, thumbExtent), qRgba(255,255, 255, 0));
    painter.drawImage(QPoint((thumbExtent - image.width())/2, (thumbExtent - image.height())/2), image);
    painter.end();

    return thumb;
}

FileThumbnails::FileThumbnails(QObject *parent) :
    QObject(parent)
{
    thread = new QThread;
    worker = new Worker;
    connect(worker, SIGNAL(done(QString,QImage)), this, SIGNAL(done(QString,QImage)));
    worker->moveToThread(thread);
    thread->start();
}

void FileThumbnails::getThumbnails(const QStringList &paths)
{
    QMetaObject::invokeMethod(worker, "getThumbnails", Qt::QueuedConnection, Q_ARG(QStringList, paths));
}
