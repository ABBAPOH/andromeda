#ifndef QTRASH_H
#define QTRASH_H

#include "io_global.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include <QMap>

#include "qtrashfileinfo.h"

class QTrashPrivate;
class IO_EXPORT QTrash : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QTrash)

public:
    explicit QTrash(QObject *parent = 0);
    ~QTrash();

    // mask : os x, linux, win
//    bool moveToTrash(const QString &path); // +++
    bool moveToTrash(const QString &path, QString *trashPath = 0); // ++? win slow parsing, but can be done
//    bool moveFromTrash(const QString &trashPath, const QString &path); // ++? on win, copy to new place and remove file in trash

    bool restore(const QString &trashPath); // -++ on mac can be done for app

//    bool copyFromTrash(const QString &trashPath, const QString &path); // ++? on win, copy to new place and remove file in trash
    bool remove(const QString &trashPath);

    QTrashFileInfoList files() const;
    QTrashFileInfoList files(const QString &trash) const;

    QStringList trashes() const;

    void clearTrash(const QString &trash);
    void clearTrash(); // +++ this is slow... on linux/mac we can notify, on win - can't...

signals:

public slots:

private:
    QTrashPrivate *d_ptr;
};

#endif // QTRASH_H
