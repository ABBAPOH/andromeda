#include "qdriveinfo.h"
#include "qdriveinfo_p.h"

QDriveInfoPrivate::QDriveInfoPrivate()
    : data(new Data)
{
}

QDriveInfoPrivate::QDriveInfoPrivate(QDriveInfoPrivate *other)
    : data(other->data)
{
}

/*!
    \class QDriveInfo
    \reentrant
    \brief QDriveInfo provides information about currently mounted drives and volumes.

    \ingroup io
    \ingroup shared

    QDriveInfo provides information about currently mounted drives and volumes.
    It alows to retrieve information about drive's space, it's mount point, label, filesystem name and type.

    You can create QDriveInfo and pass path to drive's mount point as a constructor parameter,
    or you can set it via setRootPath() method. Also, you can get list of all mounted filesystems
    using drives() method.

    QDriveInfo always caches the retreived information but you can call refresh() to invalidate the cache.
*/

/*!
    \enum QDriveInfo::DriveType
    This enum describes the type of drive or volume

    \value InvalidDrive          Drive type cannot be determined.
    \value InternalDrive         Is internal mass storage drive like a harddrive.
    \value RemovableDrive        Is a removable disk like flash disk or MMC.
    \value RemoteDrive           Is a network drive.
    \value CdromDrive            Is a cd rom or dvd drive.
    \value InternalFlashDrive    Is an internal flash disk, or Phone Memory.
    \value RamDrive              Is a virtual drive made in RAM memory.
*/

/*!
    Constructs an empty QDriveInfo object.

    This object is not ready for use, invalid and all it's parameters are empty.

    \sa setRootPath(), ready(), valid()
*/
QDriveInfo::QDriveInfo()
    : d_ptr(new QDriveInfoPrivate)
{
}

/*!
    Constructs a new QDriveInfo that gives information about the drive, mounted at
    \a rootPath.

    If passed folder or a file, will contain information about volume that folder or file located on.
    You can check if \a rootPath is correct using valid() method.

    \sa setRootPath()
*/
QDriveInfo::QDriveInfo(const QString &rootPath)
    : d_ptr(new QDriveInfoPrivate)
{
    d_ptr->data->rootPath = rootPath;
}

/*!
    Constructs a new QDriveInfo that is a copy of the given \a other QDriveInfo.
*/
QDriveInfo::QDriveInfo(const QDriveInfo &other)
    : d_ptr(new QDriveInfoPrivate(other.d_ptr))
{
}

/*!
    Destroys the QDriveInfo and frees its resources.
*/
QDriveInfo::~QDriveInfo()
{
    delete d_ptr;
}

/*!
    Makes a copy of the given \a other QDriveInfo and assigns it to this QDriveInfo.
*/
QDriveInfo &QDriveInfo::operator=(const QDriveInfo &other)
{
    if (this != &other)
        d_ptr->data = other.d_ptr->data;
    return *this;
}

/*!
    \fn bool QDriveInfo::operator!=(const QDriveInfo &other) const

    Returns true if this QDriveInfo object refers to a different drive or volume
    than the one specified by \a other; otherwise returns false.

    \sa operator==()
*/

/*!
    Returns true if this QDriveInfo object refers to a drive or volume
    that is the same as \a other; otherwise returns false.

    Note that the result of comparing two invalid QDriveInfo objects
    is always positive.

    \sa operator!=()
*/
bool QDriveInfo::operator==(const QDriveInfo &other) const
{
    if (d_ptr == other.d_ptr || d_ptr->data == other.d_ptr->data)
        return true;
    return device() == other.device();
}

/*!
    Returns mount point of the filesystem this QDriveInfo object represents.

    On Windows, in case the drive is not mounted to folder returns drive letter.

    Note that the value returned by rootPath() is the real mount point of a drive
    and may not equal to value passed to constructor or setRootPath() method.
    For example, if you have only the root drive in the system and pass '/folder'
    to setRootPath(), then this method will return '/'.

    \sa setRootPath(), device()
*/
QString QDriveInfo::rootPath() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedRootPathFlag);
    return d_func()->data->rootPath;
}

/*!
    Sets QDriveInfo to filesystem mounted at \a rootPath.

    You can also pass a path to the folder on the drive, in that case the rootPath()
    will be truncated to match the drive's mount point.

    \sa rootPath()
*/
void QDriveInfo::setRootPath(const QString &rootPath)
{
    Q_D(QDriveInfo);

    if (d->data->rootPath == rootPath)
        return;

    d->data.detach();
    d->data->clear();
    d->data->rootPath = rootPath;
}

/*!
    Returns size (in bytes) available for current user (not root).

    This size can be less than available size (exept for Symbian OS where these sizes are always equal).

    \sa bytesTotal(), bytesFree()
*/
quint64 QDriveInfo::bytesAvailable() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedBytesAvailableFlag);
    return d_func()->data->bytesAvailable;
}

/*!
    Returns free size (in bytes) available on drive. Note, that if there is some kind
    of quotas on the filesystem, this value can be bigger than bytesAvailable()

    \sa bytesTotal(), bytesAvailable()
*/
quint64 QDriveInfo::bytesFree() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedBytesFreeFlag);
    return d_func()->data->bytesFree;
}

/*!
    Returns maximum drive size in bytes.

    \sa bytesFree(), bytesAvailable()
*/
quint64 QDriveInfo::bytesTotal() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedBytesTotalFlag);
    return d_func()->data->bytesTotal;
}

/*!
    Returns the name of filesystem.

    This is not a platform-independent function, and filesystem names can vary between different
    operation systems. For example, on Windows filesystem can be named as 'NTFS' and on Linux
    as 'ntfs-3g' or 'fuseblk'.

    \sa name()
*/
QByteArray QDriveInfo::fileSystemName() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedFileSystemNameFlag);
    return d_func()->data->fileSystemName;
}

/*!
    Returns the device for this drive.

    The result of this function is platform-dependent and usually should not be used. However,
    you can retrieve this value for some platform-specific notes. By example, you can get device
    on Unix and try to read from it manually.

    On Unix filesystems (including Mac OS), this returns devpath like '/dev/sda0' for local drives.

    On Windows, returns UNC path starting with \\?\ for local drives (i.e. volume GUID).

    On Symbian OS, the first byte of the returned byte array is a drive number.

    \sa rootPath()
*/
QByteArray QDriveInfo::device() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedDeviceFlag);
    return d_func()->data->device;
}

/*!
    Returns human-readable name of a filesystem, usually called 'label'.

    Not all filesystems support this feature, in this case value returned by this method could
    be empty. Also, empty string is returned if no label set for drive.

    On Linux, retrieving the drive's label requires udev to be present in the system.

    \sa fileSystemName()
*/
QString QDriveInfo::name() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedNameFlag);
    return d_func()->data->name;
}

/*!
    \fn bool QDriveInfo::isRoot() const

    Returns true if this QDriveInfo represents a system root volume or drive; false otherwise.

    \sa rootDrive()
*/

/*!
    Returns true if the current filesystem is protected for writing; false otherwise.
*/
bool QDriveInfo::isReadOnly() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedReadOnlyFlag);
    return d_func()->data->readOnly;
}

/*!
    Returns true if current filesystem is ready to work; false otherwise.

    Note that fileSystemName(), name(), bytesTotal(), bytesFree(), and bytesAvailable()
    will return an invalid data until drive is ready.

    \sa isValid()
*/
bool QDriveInfo::isReady() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedReadyFlag);
    return d_func()->data->ready;
}

/*!
    Returns true if QDriveInfo specified by rootPath exists and mounted correctly.

    \sa isReady()
*/
bool QDriveInfo::isValid() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedValidFlag);
    return d_func()->data->valid;
}

/*!
    Returns the type of filesystem (ie remote drive, removable and so on).

    \sa QDriveInfo::DriveType
*/
QDriveInfo::DriveType QDriveInfo::type() const
{
    const_cast<QDriveInfoPrivate*>(d_func())->doStat(QDriveInfoPrivate::CachedTypeFlag);
    return QDriveInfo::DriveType(d_func()->data->type);
}

/*!
    Resets QDriveInfo inner cache.

    QDriveInfo caches information about drives to speed up performance. Some information can be retrieved
    by only 1 native function call, so, if you call bytesTotal(), QDriveInfo will also cache information
    for bytesAvailable() and bytesFree(). Also, QDriveInfo won't update information for future calls and
    you have to manually reset cache when needed.
*/
void QDriveInfo::refresh()
{
    d_func()->data->clear();
}

/*!
    Returns list of QDriveInfo's that corresponds to list of currently mounted filesystems.

    On Windows, this returnes drives presented in 'My Computer' folder. On Unix operation systems,
    returns list of all mounted filesystems (exept for Mac, where devfs is ignored). In Linux, you
    will get a lot of pseudo filesystems by calling this function, but you can filter them out
    by using type() (as they always have an InvalidDrive type).

    \sa rootDrive()
*/
QList<QDriveInfo> QDriveInfo::drives()
{
    return QDriveInfoPrivate::drives();
}

/*!
    Returns a QDriveInfo object that represents a system root volume or drive.

    On Unix systems this call returns '/' volume, on Windows - volume where operating system is installed.

    \sa isRoot()
*/
QDriveInfo QDriveInfo::rootDrive()
{
    static QDriveInfo rootDrive;
    if (!rootDrive.isValid())
        rootDrive = QDriveInfoPrivate::rootDrive();
    return rootDrive;
}
