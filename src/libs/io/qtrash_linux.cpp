#include "qtrash.h"
#include "qtrash_p.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <sys/stat.h>
#include <unistd.h>

#include "qdriveinfo.h"
#include "qtrashfileinfo_p.h"
#include "qxdg.h"

static inline QString getHomeTrash()
{
    return dataHome(true) + QLatin1Char('/') + QLatin1String("Trash");
}

static inline QString getUserId()
{
    return QString::number(getuid(), 10);
}

static inline QString getAdminTrash(const QString &drive)
{
    return drive + QLatin1Char('/') + QLatin1String(".Trash");
}

static inline QString getAdminUidTrash(const QString &drive)
{
    return drive + QLatin1Char('/') + QLatin1String(".Trash") + QLatin1Char('/') + getUserId();
}

static inline QString getUidTrash(const QString &drive)
{
    return drive + QLatin1Char('/') + QLatin1String(".Trash-") + getUserId();
}

static inline QString getInfoPath(const QString &trash)
{
    return trash + QLatin1Char('/') + QLatin1String("info");
}

static inline QString getInfoPath(const QString &trash, const QString &fileName)
{
    return trash + QLatin1Char('/') + QLatin1String("info") + QLatin1Char('/') + fileName + ".trashinfo";
}

static inline QString getFilesPath(const QString &trash)
{
    return trash + QLatin1Char('/') + QLatin1String("files");
}

static QString getFilesPath(const QString &trash, const QString &file)
{
    QDir dir(trash);
    QFileInfo info(file);

    QString baseName = info.baseName();
    QString suffix = info.suffix();
    QString newName = baseName + "." + suffix;

    for (int i = 1; dir.exists(newName); i++)
        newName = baseName + " " + QString::number(i) + "." + suffix;

    return newName;
}

static QString getTrashPath(const QString &file)
{
    QString result = file;
    result = QFileInfo(result).path();
    result = QFileInfo(result).path();
    return result;
}

static bool testDir(const QString &path, bool strongPermisions = false)
{
    QFileInfo info(path);

    bool ok = true;
    ok &= info.isDir();
    ok &= !info.isSymLink();
    if (strongPermisions)
        ok &= (info.permissions() & 0x777) == 0x700;
    else
        ok &= (info.permissions() & 0x700) == 0x700;

    return ok;
}

static bool testTrashSubdirs(const QString &trashPath, bool strongPermisions = false)
{
    return testDir(getInfoPath(trashPath), strongPermisions) && testDir(getFilesPath(trashPath), strongPermisions);
}

static bool testDriveUserTrash(const QString &trashPath)
{
    return QFile::exists(trashPath) && testDir(trashPath) && testTrashSubdirs(trashPath);
}

static bool testAdminTrash(const QString &trashPath)
{
    // Can't use QFileInfo here since we need to test for the sticky bit
    QByteArray trashPath_c = trashPath.toUtf8();

    struct stat buff;
    const unsigned int requiredBits = S_ISVTX; // Sticky bit required
    if (lstat(trashPath_c, &buff) == 0) {
        if ( (S_ISDIR(buff.st_mode)) // must be a dir
             && (!S_ISLNK(buff.st_mode)) // not a symlink
             && ((buff.st_mode & requiredBits) == requiredBits)
             && (::access(trashPath_c, W_OK) == 0) // must be user-writable
            ) {
            uid_t uid = getuid();
            const QString trashDir = trashPath + QLatin1Char('/') + QString::number(uid);
            const QByteArray trashDir_c = QFile::encodeName(trashDir);
            if (lstat(trashDir_c, &buff) == 0) {
                if ( (buff.st_uid == uid) // must be owned by user
                     && (S_ISDIR(buff.st_mode)) // must be a dir
                     && (!S_ISLNK(buff.st_mode)) // not a symlink
                     && (buff.st_mode & 0777) == 0700 ) { // rwx for user
                    return true;
                }
                qDebug() << "Directory " << trashDir << " exists but didn't pass the security checks, can't use it";
            }
        } else {
            qDebug() << "Root trash dir " << trashPath << " exists but didn't pass the security checks, can't use it";
        }
    }

    return false;
}

static void readInfoFile(const QString &infoPath, QTrashFileInfoData &data)
{
    if (!QFileInfo(infoPath).exists())
        return;

    QSettings info(infoPath, QSettings::IniFormat);
    info.beginGroup(QLatin1String("Trash Info"));

    data.originalPath = QString::fromUtf8(QByteArray::fromPercentEncoding(info.value(QLatin1String("Path")).toString().toAscii()).data());
    if (!QFileInfo(data.originalPath).isAbsolute())
        data.originalPath = QDriveInfo(infoPath).rootPath() + '/' + data.originalPath;
    data.deletionDateTime = QDateTime::fromString(info.value(QLatin1String("DeletionDate")).toString(), Qt::ISODate);
    data.size = QFileInfo(data.path).size();
}

static bool writeInfoFile(const QString &infoPath, QTrashFileInfoData &data)
{
    QFile file(infoPath);
    if ( !file.open( QIODevice::WriteOnly | QIODevice::Text ) ) {
        return false;
    }

    QTextStream out(&file);
    out << "[Trash Info]" << endl;
    out << "Path=" << QUrl::toPercentEncoding(data.originalPath, "/" ) << endl;
    out << "DeletionDate=" << data.deletionDateTime.toString(Qt::ISODate) << endl;
    return true;
}

static bool initDir(const QString &path, bool strongPermisions)
{
    if (QFile::exists(path)) {
        if (testDir(path, strongPermisions))
            return true;
        else
            void();// try rename
    }

    if (!QDir().mkpath(path))
        return false;

    if (!QFile::setPermissions(path, QFile::Permissions(0x700)))
        return false;

    if (!testDir(path, strongPermisions))
        return false;

    return true;
}

bool initSubdirs(const QString &path, bool strongPermisions)
{
    return initDir(getInfoPath(path), strongPermisions) && initDir(getFilesPath(path), strongPermisions);
}

bool initTrash(const QString &trash, bool strongPermissions)
{
    if (!initDir(trash, strongPermissions))
        return false;

    if (!initSubdirs(trash, strongPermissions))
        return false;

    return true;
}

static QString findTrash(const QString &origPath)
{
    QString trash;
    QDriveInfo homeDrive(QDir::homePath());
    QDriveInfo fileDrive(origPath);

    if (fileDrive == homeDrive) {
        trash = getHomeTrash();
        if (initTrash(trash, true))
            return trash;
    } else {
        QString rootPath = fileDrive.rootPath();

        // (1) Administrator-created $topdir/.Trash directory
        QString adminTrash = getAdminTrash(rootPath);
        if (testAdminTrash(adminTrash)) {
            trash = getAdminUidTrash(rootPath);
            if (initTrash(trash, true))
                return trash;
        }

        // (2) $topdir/.Trash-$uid
        trash = getUidTrash(rootPath);
        if (initTrash(trash, false))
            return trash;
    }

    return QString();
}

QTrashPrivate::QTrashPrivate()
{
}

bool QTrash::moveToTrash(const QString &path, QString *trashPath)
{
    QString localPath = path;
    QString trash = findTrash(path);

    if (trash.isEmpty())
        return false;

    if (trash != getHomeTrash())
        localPath = path.mid(QDriveInfo(path).rootPath().length());

    QTrashFileInfoData data;
    data.originalPath = localPath;
    data.deletionDateTime = QDateTime::currentDateTime();
//    data.size = QFileInfo(path.size());
    QString filesPath = getFilesPath(trash);
    QString newName = getFilesPath(filesPath, path);
    QString infoPath = getInfoPath(trash, newName);

    bool ok = QFile::rename(path, filesPath + '/' + newName);
    if (!ok)
        return false;

    ok &= writeInfoFile(infoPath, data);
    if (trashPath)
        *trashPath = filesPath + '/' + newName;

    return ok;
}

bool QTrash::restore(const QString &trashPath)
{
    QString fileName = QFileInfo(trashPath).fileName();
    QString trash = getTrashPath(trashPath);
    QString infoPath = getInfoPath(trash, fileName);
    QTrashFileInfoData data;
    readInfoFile(infoPath, data);

    if (data.originalPath.isEmpty())
        return false;

    bool ok = QFile::rename(trashPath, data.originalPath);
    if (!ok)
        return false;

    // if this fails, it is ok, we already restored file
    QFile::remove(infoPath);

    return ok;
}

bool QTrash::remove(const QString &trashPath)
{
    Q_D(QTrash);

    bool ok = d->removePath(trashPath);
    if (ok) {
        ok = QFile::remove(getInfoPath(getTrashPath(trashPath), QFileInfo(trashPath).fileName()));
    }

    return ok;
}

QTrashFileInfoList QTrash::files(const QString &trash) const
{
    QTrashFileInfoList result;

    QString filesPath = getFilesPath(trash);
    QString infoPath = getInfoPath(trash);
    QDir filesDir(filesPath);

    foreach (const QString &file, filesDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QTrashFileInfoData data;

        QString filePath = filesDir.absoluteFilePath(file);
        data.path = filePath;
        readInfoFile(infoPath + '/' + file + ".trashinfo", data);
        result.append(QTrashFileInfo(data));
    }

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
        QString rootPath = drive.rootPath();
        QString adminTrash = getAdminTrash(rootPath);
        if (testAdminTrash(adminTrash))
            result.append(adminTrash);

        QString userTrash = getUidTrash(rootPath);
        if (testDriveUserTrash(userTrash))
            result.append(userTrash);
    }

    return result;
}

static QStringList listDir(const QString &path)
{
    return QDir(path).entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files | QDir::Hidden);
}

void QTrash::clearTrash(const QString &trash)
{
    // The naive implementation "delete info and files in every trash directory"
    // breaks when deleted directories contain files owned by other users.
    // We need to ensure that the .trashinfo file is only removed when the
    // corresponding files could indeed be removed

    // On the other hand, we certainly want to remove any file that has no associated
    // .trashinfo file for some reason

    bool ok = true;

    QSet<QString> unremoveableFiles;

    const QTrashFileInfoList fileInfoList = files(trash);

    foreach (const QTrashFileInfo &info, fileInfoList) {
        const QString filesPath = info.path();
        if (d_func()->removePath(filesPath)) {
            QFile::remove(getInfoPath(trash, info.name()));
        } else {
            ok = false;
            // remember not to remove this file
            unremoveableFiles.insert(filesPath);
        }
    }

    // Now do the orphaned-files cleanup
    QString filesDir = getFilesPath(trash);
    foreach (const QString &fileName, listDir(filesDir)) {
        const QString filePath = filesDir + QLatin1Char('/') + fileName;
        if (!unremoveableFiles.contains(filePath)) {
            QFile::remove(filePath);
        }
    }

    return void();
}

void QTrash::clearTrash()
{
    foreach (const QString &trash, trashes()) {
        clearTrash(trash);
    }
}
