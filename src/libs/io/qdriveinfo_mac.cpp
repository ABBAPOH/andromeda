#include "qdriveinfo_p.h"

#include <CoreServices/CoreServices.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IODVDMedia.h>

#include <sys/mount.h>

#if defined(QT_LARGEFILE_SUPPORT)
#  define QT_STATFSBUF struct statfs64
#  define QT_STATFS    ::statfs64
#else
#  define QT_STATFSBUF struct statfs
#  define QT_STATFS    ::statfs
#endif

void QDriveInfoPrivate::initRootPath()
{
    if (data->rootPath.isEmpty())
        return;

    FSRef ref;
    FSPathMakeRef((UInt8*)QFile::encodeName(data->rootPath).constData(), &ref, 0);

    data->rootPath.clear();

    FSCatalogInfo catalogInfo;
    if (FSGetCatalogInfo(&ref, kFSCatInfoVolume, &catalogInfo, 0, 0, 0) != noErr)
        return;

    HFSUniStr255 volumeName;
    FSRef rootDirectory;
    OSErr error = FSGetVolumeInfo(catalogInfo.volume,
                                  0,
                                  0,
                                  kFSVolInfoFSInfo,
                                  0,
                                  &volumeName,
                                  &rootDirectory);
    if (error == noErr) {
        CFURLRef url = CFURLCreateFromFSRef(NULL, &rootDirectory);
        CFStringRef stringRef;
        stringRef = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
        if (stringRef) {
            // TODO : use utf-16 ??
            CFIndex length = CFStringGetLength(stringRef) + 1;
            char *volname = NewPtr(length);
            CFStringGetCString(stringRef, volname, length, kCFStringEncodingMacRoman);
            data->rootPath = QFile::decodeName(volname);
            CFRelease(stringRef);
            DisposePtr(volname);
        }
        CFRelease(url);

        stringRef = FSCreateStringFromHFSUniStr(NULL, &volumeName);
        if (stringRef) {
            // TODO : use utf-16 ??
            CFIndex length = CFStringGetLength(stringRef) + 1;
            char *volname = NewPtr(length);
            CFStringGetCString(stringRef, volname, length, kCFStringEncodingMacRoman);
            data->name = QFile::decodeName(volname);
            CFRelease(stringRef);
            DisposePtr(volname);
        }
    }
}

static inline QDriveInfo::DriveType determineType(const QByteArray &device)
{
    QDriveInfo::DriveType drivetype = QDriveInfo::InvalidDrive;

    DASessionRef sessionRef;
    DADiskRef diskRef;
    CFDictionaryRef descriptionDictionary;

    sessionRef = DASessionCreate(NULL);
    if (sessionRef == NULL)
        return QDriveInfo::InvalidDrive;

    diskRef = DADiskCreateFromBSDName(NULL, sessionRef, device.constData());
    if (diskRef == NULL) {
        CFRelease(sessionRef);
        return QDriveInfo::InvalidDrive;
    }

    descriptionDictionary = DADiskCopyDescription(diskRef);
    if (descriptionDictionary == NULL) {
        CFRelease(diskRef);
        CFRelease(sessionRef);
        return QDriveInfo::RemoteDrive;
    }

    CFBooleanRef boolRef;
    boolRef = (CFBooleanRef)CFDictionaryGetValue(descriptionDictionary,
                                                 kDADiskDescriptionVolumeNetworkKey);
    if (boolRef && CFBooleanGetValue(boolRef)){
        CFRelease(descriptionDictionary);
        CFRelease(diskRef);
        CFRelease(sessionRef);
        return QDriveInfo::RemoteDrive;
    }

    boolRef = (CFBooleanRef)CFDictionaryGetValue(descriptionDictionary,
                                                 kDADiskDescriptionMediaRemovableKey);
    if (boolRef)
        drivetype = CFBooleanGetValue(boolRef) ? QDriveInfo::RemovableDrive : QDriveInfo::InternalDrive;

    DADiskRef wholeDisk;
    wholeDisk = DADiskCopyWholeDisk(diskRef);
    if (wholeDisk) {
        io_service_t mediaService;
        mediaService = DADiskCopyIOMedia(wholeDisk);
        if (mediaService) {
            if (IOObjectConformsTo(mediaService, kIOCDMediaClass)
                || IOObjectConformsTo(mediaService, kIODVDMediaClass)) {
                drivetype = QDriveInfo::CdromDrive;
            }
            IOObjectRelease(mediaService);
        }
        CFRelease(wholeDisk);
    }

    CFRelease(descriptionDictionary);
    CFRelease(diskRef);
    CFRelease(sessionRef);

    return drivetype;
}

void QDriveInfoPrivate::doStat(uint requiredFlags)
{
    if (data->getCachedFlag(requiredFlags))
        return;

    if (!data->getCachedFlag(CachedRootPathFlag | CachedNameFlag)) {
        initRootPath();
        data->setCachedFlag(CachedRootPathFlag | CachedNameFlag);
    }

    if (data->rootPath.isEmpty() || (data->getCachedFlag(CachedValidFlag) && !data->valid))
        return;

    if (!data->getCachedFlag(CachedValidFlag))
        requiredFlags |= CachedValidFlag; // force drive validation

    if (requiredFlags & CachedTypeFlag)
        requiredFlags |= CachedDeviceFlag;


    uint bitmask = 0;

    bitmask = CachedDeviceFlag | CachedFileSystemNameFlag |
              CachedBytesTotalFlag | CachedBytesFreeFlag | CachedBytesAvailableFlag |
              CachedReadOnlyFlag | CachedReadyFlag | CachedValidFlag;
    if (requiredFlags & bitmask) {
        getVolumeInfo();
        data->setCachedFlag(bitmask);

        if (!data->valid)
            return;
    }

    bitmask = CachedTypeFlag;
    if (requiredFlags & bitmask) {
        data->type = determineType(data->device);
        data->setCachedFlag(bitmask);
    }
}

void QDriveInfoPrivate::getVolumeInfo()
{
    QT_STATFSBUF statfs_buf;
    int result = QT_STATFS(QFile::encodeName(data->rootPath).constData(), &statfs_buf);
    if (result == 0) {
        data->valid = true;
        data->ready = true;

        data->device = QByteArray(statfs_buf.f_mntfromname);
        data->fileSystemName = QByteArray(statfs_buf.f_fstypename);

        data->bytesTotal = statfs_buf.f_blocks * statfs_buf.f_bsize;
        data->bytesFree = statfs_buf.f_bfree * statfs_buf.f_bsize;
        data->bytesAvailable = statfs_buf.f_bavail * statfs_buf.f_bsize;

        data->readOnly = (statfs_buf.f_flags & MNT_RDONLY);
    }
}

QList<QDriveInfo> QDriveInfoPrivate::drives()
{
    QList<QDriveInfo> drives;

    OSErr result = noErr;
    for (ItemCount volumeIndex = 1; result == noErr || result != nsvErr; volumeIndex++) {
        FSVolumeRefNum actualVolume;
        FSRef rootDirectory;
        result = FSGetVolumeInfo(kFSInvalidVolumeRefNum,
                                 volumeIndex,
                                 &actualVolume,
                                 0,
                                 0,
                                 0,
                                 &rootDirectory);
        if (result == noErr) {
            CFURLRef url = CFURLCreateFromFSRef(NULL, &rootDirectory);
            CFStringRef stringRef = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
            if (stringRef) {
                CFIndex length = CFStringGetLength(stringRef) + 1;
                char *volname = NewPtr(length);
                CFStringGetCString(stringRef, volname, length, kCFStringEncodingMacRoman);
                {
                    QDriveInfo drive;
                    drive.d_ptr->data->rootPath = QFile::decodeName(volname);
                    drive.d_ptr->data->setCachedFlag(CachedRootPathFlag);
                    drives.append(drive);
                }
                CFRelease(stringRef);
                DisposePtr(volname);
            }
            CFRelease(url);
        }
    }

    return drives;
}

QDriveInfo QDriveInfoPrivate::rootDrive()
{
    return QDriveInfo(QLatin1String("/"));
}
