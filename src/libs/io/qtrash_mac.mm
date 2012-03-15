#include "qtrash.h"
#include "qtrash_p.h"

#include <CoreFoundation/CFError.h>
#include <CoreServices/CoreServices.h>
#include <AppKit/NSWorkspace.h>
#include <Foundation/NSString.h>

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>

#include "qdriveinfo.h"
#include "qtrashfileinfo_p.h"

static inline QString getHomeTrash()
{
    return QDir().homePath() + QLatin1Char('/') + QLatin1String(".Trash");
}

static inline QString getUserId()
{
    return QString::number(getuid(), 10);
}

static QString getDriveTrash(const QString &drive)
{
    return drive + QLatin1Char('/') + QLatin1String(".Trashes") + QLatin1Char('/') + getUserId();
}

static void notifyFinder(const QString &trash)
{
    QByteArray utf8TrashPath = trash.toUtf8();

    // Notify Finder about moving file to trash
    // should i receive top trash dir or not?
    NSString *trashPath = [[NSString alloc] initWithBytes:utf8TrashPath.data()
            length:utf8TrashPath.length()
            encoding:NSUTF8StringEncoding];
    [[NSWorkspace sharedWorkspace] noteFileSystemChanged:trashPath];
    [trashPath release];
}

QMutex m_mutex;
QMap<QString, QTrashFileInfo> QTrashPrivate::deletedFiles;

QTrashPrivate::QTrashPrivate()
{
}

bool QTrash::moveToTrash(const QString &path, QString *newFilePath)
{
    Q_D(QTrash);

    QByteArray pathUtf8 = path.toUtf8();
    char *newPathUtf8 = 0;
    OSStatus result = FSPathMoveObjectToTrashSync(pathUtf8.constData(),
                                                  &newPathUtf8,
                                                  kFSFileOperationDoNotMoveAcrossVolumes);

    QString newPath = QString::fromUtf8(newPathUtf8);
    free(newPathUtf8);

    QTrashFileInfoData data;
    data.originalPath = path;
    data.path = newPath;
    data.deletionDateTime = QDateTime::currentDateTime();
    data.size = QFileInfo(newPath).size();
    m_mutex.lock();
    d->deletedFiles.insert(newPath, QTrashFileInfo(data));
    m_mutex.unlock();

    notifyFinder(QFileInfo(newPath).path());

    if (newFilePath)
        *newFilePath = newPath;

    return result == noErr;
}

bool QTrash::restore(const QString &trashPath)
{
    Q_D(QTrash);

    m_mutex.lock();
    QString originalPath = d->deletedFiles.value(trashPath).originalPath();
    m_mutex.unlock();
    if (originalPath.isEmpty())
        return false;

    bool ok = QFile::rename(trashPath, originalPath);
    if (ok) {
        m_mutex.lock();
        d->deletedFiles.remove(trashPath);
        m_mutex.unlock();
        notifyFinder(QFileInfo(trashPath).path());
    }

    return ok;
}

bool QTrash::remove(const QString &trashPath)
{
    Q_D(QTrash);

    bool ok = d->removePath(trashPath);
    if (ok) {
        m_mutex.lock();
        d->deletedFiles.remove(trashPath);
        m_mutex.unlock();
        notifyFinder(QFileInfo(trashPath).path());
    }

    return ok;
}

QTrashFileInfoList QTrash::files(const QString &trash) const
{
    Q_D(const QTrash);

    QTrashFileInfoList result;
    QStringList paths;

    QDir trashDir(trash);
    foreach (const QString &file, trashDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QString filePath = trashDir.absoluteFilePath(file);
        paths.append(filePath);

        m_mutex.lock();
        if (d->deletedFiles.contains(filePath)) {
            result.append(d->deletedFiles.value(filePath));
            m_mutex.unlock();
        } else {
            m_mutex.unlock();
            QTrashFileInfoData data;
            // we can't determine original path here
            data.path = filePath;
            data.size = QFileInfo(trashDir.absoluteFilePath(file)).size();
            result.append(QTrashFileInfo(data));
        }
    }

    // update deleted files if file was deleted from elsewhere
    m_mutex.lock();
    foreach (const QString &path, d->deletedFiles.keys()) {
        if (path.startsWith(trash)) { // file within current trash
            if (!paths.contains(path)) {
                d->deletedFiles.remove(path);
            }
        }
    }
    m_mutex.unlock();

    return result;
}

QTrashFileInfoList QTrash::files() const
{
    QTrashFileInfoList result;

    foreach (const QString &trashPath, trashes()) {
        result.append(files(trashPath));
    }

    return result;
}

QStringList QTrash::trashes() const
{
    QStringList result;

    QString homeTrash = getHomeTrash();
    if (QFileInfo(homeTrash).exists())
        result.append(homeTrash);

    foreach (const QDriveInfo &drive, QDriveInfo::drives()) {
        QString driveTrash = getDriveTrash(drive.rootPath());
        QFileInfo info(driveTrash);
        if (info.exists() && (info.permissions() & 0x600) == 0x600) { // user can read and write to dir
            result.append(info.absoluteFilePath());
        }
    }

    return result;
}

void QTrash::clearTrash(const QString &trash)
{
    foreach (const QTrashFileInfo &info, files(trash)) {
        remove(info.path());
    }

    notifyFinder(trash);
}

void QTrash::clearTrash()
{
    foreach (const QTrashFileInfo &info, files()) {
        remove(info.path());
    }
}
