#ifndef FILETHUMBNAILS_H
#define FILETHUMBNAILS_H

#include <QtCore/QObject>
#include <QtGui/QImage>

class QThread;

namespace FileManager {

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = 0);

public slots:
    void getThumbnails(const QStringList &paths);

private:
    QImage getThumb(const QString &path);

signals:
    void done(const QString &path, const QImage &thumb);
};

class FileThumbnails : public QObject
{
    Q_OBJECT
public:
    explicit FileThumbnails(QObject *parent = 0);

    void getThumbnails(const QStringList &paths);

private:
    Worker *worker;
    QThread *thread;

signals:
    void done(const QString &path, const QImage &thumb);
};

} // namespace FileManager

#endif // FILETHUMBNAILS_H
