#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

QDriveWatcher::QDriveWatcher(QObject *parent) :
    QObject(parent),
    startStopCounter(0),
    engine(0)
{
}

QDriveWatcher::~QDriveWatcher()
{
    if (startStopCounter != 0)
        qWarning("QDriveWatcher is going to be deleted but it seems like it is still in use.");

    stop_sys();
}

void QDriveWatcher::start()
{
    startStopCounter.ref();
    if (startStopCounter == 1) {
        if (!start_sys())
            stop();
    }
}

void QDriveWatcher::stop()
{
    if (!startStopCounter.deref())
        stop_sys();
}

Q_GLOBAL_STATIC(QDriveWatcher, theWatcher)

/*!
    \class QDriveController
    \brief The QDriveController class provides an interface for drives in operating system.
    \reentrant

    QDriveController monitors appearing and disappearing of new drives and system and allows
    to mount and unmout filesystems. On most operating systems when user inserts flash drive
    or a cd-rom, it is automatically mounted to filesystem and you can monitor this events
    using this class.

    The driveMounted() signal is emitted when drive is mounted to filesystem - when user
    inserts cd-rom or adds new network drive.

    The driveUnmounted() signal is emitted when drive is unmounted from filesystem.
*/

/*!
    Constructs a new drive controller object with the given \a parent.
*/
QDriveController::QDriveController(QObject *parent) :
    QObject(parent), d(new QDriveControllerPrivate)
{
    if (QDriveWatcher *watcher = theWatcher()) {
        connect(watcher, SIGNAL(driveAdded(QString)),
                this, SIGNAL(driveMounted(QString)), Qt::QueuedConnection);
        connect(watcher, SIGNAL(driveRemoved(QString)),
                this, SIGNAL(driveUnmounted(QString)), Qt::QueuedConnection);

        watcher->start();
    }
}

/*!
    Destroys the drive watcher.
*/
QDriveController::~QDriveController()
{
    if (QDriveWatcher *watcher = theWatcher())
        watcher->stop();

    delete d;
}

/*!
    Returns a system error code for last operation.
*/
int QDriveController::error() const
{
    return d->error.code;
}

/*!
    Returns a description of an error occured for last operation.
*/
QString QDriveController::errorString() const
{
    return d->error.string;
}

/*!
    \fn bool QDriveController::mount(const QString &device, const QString &path = QString());
    \brief Mounts specified \a device at \a path

    This function performs mounting operation with the given device. Device can be a GUID or UNC path in windows,
    filesystem entry located in /dev in Unix or network path (smb:// or afp://) on Mac.

    \note due to implementation, \a path parameter is ignored for Unix operating systems and is only available on
    Windows. If empty path is passed on Windows, QDriveController tries to find not used drive letter and mounts
    using it as path.

    \sa driveMounted
*/

/*!
    \fn bool QDriveController::unmount(const QString &path);
    \brief Unmounts device mounted at \a path

    This function does not remove device from operating system. To completely remove device, use eject.

    \sa driveUnmounted
*/

/*!
    \fn bool QDriveController::eject(const QString &path);
    \brief Unmounts and ejects device mounted at \a path

    This function removes device from operating system, ejecting cd-rom. After using this function, you can't mount
    ejected device.
*/

/*!
    \fn void QDriveController::driveMounted(const QString &path);

    This signal is emitted when the new drive is mounted at \a path.
    Use QDriveInfo class to get information about this drive.

    \sa driveUnmounted()
*/

/*!
    \fn void QDriveController::driveUnmounted(const QString &path);

    This signal is emitted when the drive is mounted at \a path becomes unmounted.

    \sa driveMounted()
*/
