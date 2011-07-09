#include "qdriveinfo_p.h"

#include <QtCore/QDirIterator>
#include <QtCore/QTextStream>

#include <errno.h>
#include <mntent.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

#ifndef USE_PRIVATE_HEADERS
#  undef EINTR_LOOP
#  define EINTR_LOOP(var, cmd)                  \
    do {                                        \
        var = cmd;                              \
    } while (var == -1 && errno == EINTR)
#else
#  include <private/qcore_unix_p.h>
#endif

#if defined(QT_LARGEFILE_SUPPORT)
#  define QT_STATFSBUF struct statvfs64
#  define QT_STATFS    ::statvfs64
#else
#  define QT_STATFSBUF struct statvfs
#  define QT_STATFS    ::statvfs
#endif

#ifndef _PATH_MOUNTED
#  define _PATH_MOUNTED "/etc/mtab"
#endif
#ifndef _PATH_DISK_BY_LABEL
#  define _PATH_DISK_BY_LABEL "/dev/disk/by-label"
#endif


void QDriveInfoPrivate::initRootPath()
{
    if (data->rootPath.isEmpty())
        return;

    FILE *fp = ::setmntent(_PATH_MOUNTED, "r");
    if (fp) {
        quint32 maxLength = 0;
        QString oldRootPath = data->rootPath;
        data->rootPath.clear();

        struct mntent *mnt;
        while ((mnt = ::getmntent(fp))) {
            QString mountDir = QFile::decodeName(mnt->mnt_dir);
            // we try to find most suitable entry
            if (oldRootPath.startsWith(mountDir) && maxLength < (quint32)mountDir.length()) {
                maxLength = mountDir.length();
                data->rootPath = mountDir;
                data->device = QByteArray(mnt->mnt_fsname);
                data->fileSystemName = QByteArray(mnt->mnt_type);
            }
        }
        ::endmntent(fp);
    }
}

static inline QDriveInfo::DriveType determineType(const QByteArray &device)
{
    QString dmFile;

    if (device.contains("mapper")) {
        QT_STATBUF stat_buf;
        int result;
        EINTR_LOOP(result, QT_STAT(device.constData(), &stat_buf));
        if (result == 0)
            dmFile = QLatin1String("dm-") + QString::number(stat_buf.st_rdev & 0377);
        else
            return QDriveInfo::InvalidDrive;
    } else {
        dmFile = QString(device).section(QLatin1Char('/'), 2, 3);
        if (dmFile.startsWith(QLatin1String("mmc"))) {
            // assume this dev is removable sd/mmc card.
            return QDriveInfo::RemovableDrive;
        }

        if (dmFile.length() > 3) {
            // if device has number, we need the 'parent' device
            dmFile.chop(1);
            if (dmFile.endsWith(QLatin1Char('p')))
                dmFile.chop(1); // get rid of partition number
        }
    }
    dmFile = QLatin1String("/sys/block/") + dmFile + QLatin1String("/removable");

    QFile file(dmFile);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream sysinfo(&file);
        QString line = sysinfo.readAll();
        if (line.contains(QLatin1Char('1')))
            return QDriveInfo::RemovableDrive;
    }

    if (device.startsWith("/dev"))
        return QDriveInfo::InternalDrive;

    return QDriveInfo::InvalidDrive;
}

// we need udev to be present in system to get label name
// Unfortunately, i don't know proper way to get labels except this. Maybe libudev can provide
// this information. TODO: explore it
// If we have udev installed in the system, proper symlinks are created by it, so we don't
// need to link to libudev
// If not, i don't know other way to get labels without root privelegies
static inline QString getName(const QByteArray &device)
{
    QDirIterator it(QLatin1String(_PATH_DISK_BY_LABEL), QDir::NoDotAndDotDot);
    while (it.hasNext()) {
        it.next();
        QFileInfo fileInfo(it.fileInfo());
        if (fileInfo.isSymLink() && fileInfo.symLinkTarget().toLatin1() == device)
            return fileInfo.fileName();
    }

    return QString();
}

void QDriveInfoPrivate::doStat(uint requiredFlags)
{
    if (data->getCachedFlag(requiredFlags))
        return;

    if (!data->getCachedFlag(CachedRootPathFlag | CachedDeviceFlag | CachedFileSystemNameFlag)) {
        initRootPath();
        data->setCachedFlag(CachedRootPathFlag | CachedDeviceFlag | CachedFileSystemNameFlag);
    }

    if (data->rootPath.isEmpty() || (data->getCachedFlag(CachedValidFlag) && !data->valid))
        return;

    if (!data->getCachedFlag(CachedValidFlag))
        requiredFlags |= CachedValidFlag; // force drive validation


    uint bitmask = 0;

    bitmask = CachedBytesTotalFlag | CachedBytesFreeFlag | CachedBytesAvailableFlag |
              CachedReadOnlyFlag | CachedReadyFlag | CachedValidFlag;
    if (requiredFlags & bitmask) {
        getVolumeInfo();
        data->setCachedFlag(bitmask);

        if (!data->valid)
            return;
    }

    bitmask = CachedNameFlag;
    if (requiredFlags & bitmask) {
        data->name = getName(data->device);
        data->setCachedFlag(bitmask);
    }

    bitmask = CachedTypeFlag;
    if (requiredFlags & bitmask) {
        data->type = determineType(data->device);
        if (data->type == QDriveInfo::InvalidDrive) {
            // test for UNC shares
            if (data->rootPath.startsWith(QLatin1String("//"))
                || data->fileSystemName == "nfs"
                || data->fileSystemName == "cifs"
                || data->fileSystemName.startsWith("smb")) {
                data->type = QDriveInfo::RemoteDrive;
            }
        }
        data->setCachedFlag(bitmask);
    }
}

void QDriveInfoPrivate::getVolumeInfo()
{
    QT_STATFSBUF statfs_buf;
    int result;
    EINTR_LOOP(result, QT_STATFS(QFile::encodeName(data->rootPath).constData(), &statfs_buf));
    if (result == 0) {
        data->valid = true;
        data->ready = true;

        data->bytesTotal = statfs_buf.f_blocks * statfs_buf.f_bsize;
        data->bytesFree = statfs_buf.f_bfree * statfs_buf.f_bsize;
        data->bytesAvailable = statfs_buf.f_bavail * statfs_buf.f_bsize;

        data->readOnly = (statfs_buf.f_flag & ST_RDONLY);
    }
}

QList<QDriveInfo> QDriveInfoPrivate::drives()
{
    QList<QDriveInfo> drives;

    FILE *fp = ::setmntent(_PATH_MOUNTED, "r");
    if (fp) {
        struct mntent *mnt;
        while ((mnt = ::getmntent(fp))) {
            QDriveInfo drive;
            drive.d_ptr->data->rootPath = QFile::decodeName(mnt->mnt_dir);
            drive.d_ptr->data->device = QByteArray(mnt->mnt_fsname);
            drive.d_ptr->data->fileSystemName = QByteArray(mnt->mnt_type);
            drive.d_ptr->data->setCachedFlag(CachedRootPathFlag |
                                             CachedFileSystemNameFlag |
                                             CachedDeviceFlag);
            drives.append(drive);
        }
        ::endmntent(fp);
    }

    return drives;
}

QDriveInfo QDriveInfoPrivate::rootDrive()
{
    return QDriveInfo(QLatin1String("/"));
}
