#ifndef QDRIVEINFO_P_H
#define QDRIVEINFO_P_H

#include "qdriveinfo.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QFile>

#include <qplatformdefs.h>

class QDriveInfoPrivate
{
public:
    QDriveInfoPrivate();
    QDriveInfoPrivate(QDriveInfoPrivate *other);

    enum CachedFlags {
        CachedRootPathFlag = 0x001,
        CachedDeviceFlag = 0x002,
        CachedFileSystemNameFlag = 0x004,
        CachedNameFlag = 0x008,
        CachedBytesTotalFlag = 0x010,
        CachedBytesFreeFlag = 0x020,
        CachedBytesAvailableFlag = 0x040,
        CachedTypeFlag = 0x100,
        CachedReadOnlyFlag = 0x200,
        CachedReadyFlag = 0x400,
        CachedValidFlag = 0x800
    };

    struct Data : public QSharedData
    {
        Data() : QSharedData(),
            bytesTotal(0), bytesFree(0), bytesAvailable(0),
            type(QDriveInfo::InvalidDrive),
            readOnly(false), ready(false), valid(false),
            cachedFlags(0)
        {}
        Data(const Data &other) : QSharedData(other),
            rootPath(other.rootPath),
            bytesTotal(0), bytesFree(0), bytesAvailable(0),
            type(QDriveInfo::InvalidDrive),
            readOnly(false), ready(false), valid(false),
            cachedFlags(0)
        {}

        inline void clear()
        {
            device.clear();
            fileSystemName.clear();
            name.clear();

            bytesTotal = 0;
            bytesFree = 0;
            bytesAvailable = 0;

            type = QDriveInfo::InvalidDrive;
            readOnly = false;
            ready = false;
            valid = false;

            cachedFlags = 0;
        }

        inline bool getCachedFlag(uint c) const
        { return !((cachedFlags & c) ^ c); }
        inline void setCachedFlag(uint c)
        { cachedFlags |= c; }

        QString rootPath;
        QByteArray device;
        QByteArray fileSystemName;
        QString name;

        quint64 bytesTotal;
        quint64 bytesFree;
        quint64 bytesAvailable;

        ushort type : 8;
        ushort readOnly : 1;
        ushort ready : 1;
        ushort valid : 1;
        ushort reserved : 5;

        uint cachedFlags;
    };
    QExplicitlySharedDataPointer<Data> data;

    void initRootPath();
    void doStat(uint requiredFlags);

    static QList<QDriveInfo> drives();
    static QDriveInfo rootDrive();

protected:
    void getVolumeInfo();
#if defined(Q_OS_WIN)
    void getDiskFreeSpace();
#endif
};

#endif // QDRIVEINFO_P_H
