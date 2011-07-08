#include "qdriveinfo_p.h"

#include <QtCore/QDir>
#include <QtCore/QVarLengthArray>

#include <Userenv.h>

void QDriveInfoPrivate::initRootPath()
{
    if (data->rootPath.isEmpty())
        return;

    QString path = QDir::toNativeSeparators(data->rootPath);
    data->rootPath.clear();

    if (path.startsWith(QLatin1String("\\\\?\\")))
        path = path.mid(4);
    if (path.isEmpty())
        return;
    path[0] = path[0].toUpper();
    if (!(path.length() >= 2
          && path.at(0).unicode() >= 'A' && path.at(0).unicode() <= 'Z'
          && path.at(1) == QLatin1Char(':'))) {
        return;
    }
    if (!path.endsWith(QLatin1Char('\\')))
        path.append(QLatin1Char('\\'));

    // ### test if disk mounted to folder on other disk
    wchar_t buffer[MAX_PATH + 1];
    if (::GetVolumePathName((wchar_t *)path.utf16(), buffer, MAX_PATH))
        data->rootPath = QDir::fromNativeSeparators(QString::fromWCharArray(buffer));
}

static inline QByteArray getDevice(const QString &rootPath)
{
    QString path = QDir::toNativeSeparators(rootPath);
    wchar_t deviceBuffer[MAX_PATH + 1];
    if (::GetVolumeNameForVolumeMountPoint((wchar_t *)path.utf16(), deviceBuffer, MAX_PATH))
        return QString::fromWCharArray(deviceBuffer).toLatin1();

    return QByteArray();
}

static inline QDriveInfo::DriveType determineType(const QString &rootPath)
{
#if !defined(Q_OS_WINCE)
    UINT result = ::GetDriveType((wchar_t *)rootPath.utf16());
    switch (result) {
    case DRIVE_REMOVABLE:
        return QDriveInfo::RemovableDrive;

    case DRIVE_FIXED:
        return QDriveInfo::InternalDrive;

    case DRIVE_REMOTE:
        return QDriveInfo::RemoteDrive;

    case DRIVE_CDROM:
        return QDriveInfo::CdromDrive;

    case DRIVE_RAMDISK:
        return QDriveInfo::RamDrive;

    case DRIVE_UNKNOWN:
    case DRIVE_NO_ROOT_DIR:
    // fall through
    default:
        break;
    };
#else
    Q_UNUSED(rootPath)
#endif
    return QDriveInfo::InvalidDrive;
}

void QDriveInfoPrivate::doStat(uint requiredFlags)
{
    if (data->getCachedFlag(requiredFlags))
        return;

    if (!data->getCachedFlag(CachedRootPathFlag)) {
        initRootPath();
        data->setCachedFlag(CachedRootPathFlag);
    }

    if (data->rootPath.isEmpty() || (data->getCachedFlag(CachedValidFlag) && !data->valid))
        return;

    if (!data->getCachedFlag(CachedValidFlag))
        requiredFlags |= CachedValidFlag; // force drive validation


    uint bitmask = 0;

    bitmask = CachedFileSystemNameFlag | CachedNameFlag |
              CachedReadOnlyFlag | CachedReadyFlag | CachedValidFlag;
    if (requiredFlags & bitmask) {
        getVolumeInfo();
        if (data->valid && !data->ready)
            bitmask = CachedValidFlag;
        data->setCachedFlag(bitmask);

        if (!data->valid)
            return;
    }

    bitmask = CachedDeviceFlag;
    if (requiredFlags & bitmask) {
        data->device = getDevice(data->rootPath);
        data->setCachedFlag(bitmask);
    }

    bitmask = CachedBytesTotalFlag | CachedBytesFreeFlag | CachedBytesAvailableFlag;
    if (requiredFlags & bitmask) {
        getDiskFreeSpace();
        data->setCachedFlag(bitmask);
    }

    bitmask = CachedTypeFlag;
    if (requiredFlags & bitmask) {
        data->type = determineType(data->rootPath);
        data->setCachedFlag(bitmask);
    }
}

void QDriveInfoPrivate::getVolumeInfo()
{
    UINT oldmode = ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    QString path = QDir::toNativeSeparators(data->rootPath);
    wchar_t nameBuf[MAX_PATH + 1];
    DWORD fileSystemFlags = 0;
    wchar_t fileSystemNameBuf[MAX_PATH + 1];
    bool result = ::GetVolumeInformation((wchar_t *)path.utf16(),
                                         nameBuf, MAX_PATH,
                                         0, 0,
                                         &fileSystemFlags,
                                         fileSystemNameBuf, MAX_PATH);
    if (!result) {
        data->ready = false;
        data->valid = (::GetLastError() == ERROR_NOT_READY);
    } else {
        data->ready = true;
        data->valid = true;

        data->fileSystemName = QString::fromWCharArray(fileSystemNameBuf).toLatin1();
        data->name = QString::fromWCharArray(nameBuf);

        data->readOnly = (fileSystemFlags & FILE_READ_ONLY_VOLUME);
    }

    ::SetErrorMode(oldmode);
}

void QDriveInfoPrivate::getDiskFreeSpace()
{
    UINT oldmode = ::SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

    QString path = QDir::toNativeSeparators(data->rootPath);
    ::GetDiskFreeSpaceEx((wchar_t *)path.utf16(),
                         (PULARGE_INTEGER)&data->bytesAvailable,
                         (PULARGE_INTEGER)&data->bytesTotal,
                         (PULARGE_INTEGER)&data->bytesFree);

    ::SetErrorMode(oldmode);
}

QList<QDriveInfo> QDriveInfoPrivate::drives()
{
    QList<QDriveInfo> drives;

    char driveName[] = "A:/";
    quint32 driveBits = quint32(::GetLogicalDrives()) & 0x3ffffff;
    while (driveBits) {
        if (driveBits & 1) {
            QDriveInfo drive;
            drive.d_ptr->data->rootPath = QLatin1String(driveName);
            drive.d_ptr->data->setCachedFlag(CachedRootPathFlag);
            drives.append(drive);
        }
        driveName[0]++;
        driveBits = driveBits >> 1;
    }

    return drives;
}

QDriveInfo QDriveInfoPrivate::rootDrive()
{
    DWORD dwBufferSize = 128;
    QVarLengthArray<wchar_t, 128> profilesDirectory(dwBufferSize);
    bool ok;
    do {
        if (dwBufferSize > (DWORD)profilesDirectory.size())
            profilesDirectory.resize(dwBufferSize);
        ok = ::GetProfilesDirectory(profilesDirectory.data(), &dwBufferSize);
    } while (!ok && GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    if (ok)
        return QDriveInfo(QString::fromWCharArray(profilesDirectory.data(), profilesDirectory.size()));

    return QDriveInfo();
}
