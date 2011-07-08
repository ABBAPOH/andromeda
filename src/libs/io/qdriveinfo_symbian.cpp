#include "qdriveinfo_p.h"

#include <f32file.h>

// ### remove following line and include private header
Q_CORE_EXPORT RFs &qt_s60GetRFs();

void QDriveInfoPrivate::initRootPath()
{
    if (data->rootPath.isEmpty())
        return;

    QChar driveLetter = data->rootPath.at(0).toUpper();
    data->rootPath.clear();

    RFs rfs = qt_s60GetRFs();

    TInt driveNumber;
    if (RFs::CharToDrive(driveLetter.toAscii(), driveNumber) == KErrNone) {
        data->rootPath = driveLetter + QLatin1String(":/");
        data->device = QByteArray(1, driveNumber);
    }
}

void QDriveInfoPrivate::doStat(uint requiredFlags)
{
    if (data->getCachedFlag(requiredFlags))
        return;

    if (!data->getCachedFlag(CachedRootPathFlag | CachedDeviceFlag)) {
        initRootPath();
        data->setCachedFlag(CachedRootPathFlag | CachedDeviceFlag);
    }

    if (data->rootPath.isEmpty() || (data->getCachedFlag(CachedValidFlag) && !data->valid))
        return;

    if (!data->getCachedFlag(CachedValidFlag))
        requiredFlags |= CachedValidFlag; // force drive validation


    uint bitmask = CachedFileSystemNameFlag | CachedNameFlag |
                   CachedBytesTotalFlag | CachedBytesFreeFlag | CachedBytesAvailableFlag |
                   CachedTypeFlag | CachedReadOnlyFlag |
                   CachedReadyFlag | CachedValidFlag;
    if (requiredFlags & bitmask) {
        getVolumeInfo();
        if (data->valid && !data->ready)
            bitmask = CachedTypeFlag | CachedReadOnlyFlag | CachedValidFlag;
        data->setCachedFlag(bitmask);
    }
}

void QDriveInfoPrivate::getVolumeInfo()
{
    RFs rfs = qt_s60GetRFs();

    TInt drive = data->device[0];

    TDriveInfo driveInfo;
    if (rfs.Drive(driveInfo, drive) == KErrNone) {
        data->valid = true;

        TVolumeInfo volumeInfo;
        if (rfs.Volume(volumeInfo, drive) == KErrNone) {
            data->ready = true;

            data->name = QString::fromUtf16((const ushort *)volumeInfo.iName.Ptr(),
                                            volumeInfo.iName.Length());
            TFSName fileSystemNameBuf;
            if (rfs.FileSystemSubType(drive, fileSystemNameBuf) == KErrNone)
                data->fileSystemName = QString::fromUtf16((const ushort *)fileSystemNameBuf.Ptr(),
                                                          fileSystemNameBuf.Length()).toLatin1();

            data->bytesTotal = volumeInfo.iSize;
            data->bytesFree = volumeInfo.iFree;
            data->bytesAvailable = volumeInfo.iFree;
        }

        data->readOnly = (driveInfo.iMediaAtt & KMediaAttWriteProtected);

        switch (driveInfo.iType) {
        case EMediaFlash:
        case EMediaFloppy:
            data->type = QDriveInfo::RemovableDrive;
            break;
        case EMediaHardDisk:
        case EMediaRom:
            data->type = QDriveInfo::InternalDrive;
            if (driveInfo.iDriveAtt & KDriveAttRemovable)
                data->type = QDriveInfo::RemovableDrive;
            break;
        case EMediaNANDFlash:
            data->type = QDriveInfo::InternalFlashDrive;
            break;
        case EMediaRam:
            data->type = QDriveInfo::RamDrive;
            break;
        case EMediaCdRom:
            data->type = QDriveInfo::CdromDrive;
            data->readOnly = true;
            break;
        case EMediaRemote:
            data->type = QDriveInfo::RemoteDrive;
            break;
        case EMediaNotPresent:
        default:
            break;
        }
    }
}

QList<QDriveInfo> QDriveInfoPrivate::drives()
{
    QList<QDriveInfo> drives;

    RFs rfs = qt_s60GetRFs();

    char driveName[] = "A:/";
    TChar driveChar;
    TDriveList drivelist;
    if (rfs.DriveList(drivelist) == KErrNone) {
        for (TInt driveNumber = EDriveA; driveNumber <= EDriveZ; ++driveNumber) {
            if (drivelist[driveNumber] && RFs::DriveToChar(driveNumber, driveChar) == KErrNone) {
                driveName[0] = driveChar;
                QDriveInfo drive;
                drive.d_ptr->data->rootPath = QLatin1String(driveName);
                drive.d_ptr->data->device = QByteArray(1, driveNumber);
                drive.d_ptr->data->setCachedFlag(CachedRootPathFlag | CachedDeviceFlag);
                drives.append(drive);
            }
        }
    }

    return drives;
}

QDriveInfo QDriveInfoPrivate::rootDrive()
{
    RFs rfs = qt_s60GetRFs();

    char driveName[] = "A:/";
    driveName[0] = RFs::GetSystemDriveChar();
    TInt driveNumber;
    if (RFs::CharToDrive(driveName[0], driveNumber) == KErrNone) {
        QDriveInfo drive;
        drive.d_ptr->data->rootPath = QLatin1String(driveName);
        drive.d_ptr->data->device = QByteArray(1, driveNumber);
        drive.d_ptr->data->setCachedFlag(CachedRootPathFlag | CachedDeviceFlag);
        return drive;
    }

    return QDriveInfo();
}
