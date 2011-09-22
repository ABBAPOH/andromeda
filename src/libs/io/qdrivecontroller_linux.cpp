#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QSet>
#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>

#if !defined(QT_NO_UDISKS)
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusReply>
#endif // QT_NO_UDISKS

#include <sys/inotify.h>
#include <sys/mount.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <mntent.h>
#include <linux/cdrom.h>

#include  "qdriveinfo.h"

#ifndef _PATH_MOUNTED
#  define _PATH_MOUNTED "/etc/mtab"
#endif

#if !defined(QT_NO_UDISKS)
static const QString UDISKS_SERVICE(QString::fromAscii("org.freedesktop.UDisks"));
static const QString UDISKS_PATH(QString::fromAscii("/org/freedesktop/UDisks"));
static const QString UDISKS_INTERFACE(QString::fromAscii("org.freedesktop.UDisks"));
static const QString UDISKS_DEVICE_INTERFACE(QString::fromAscii("org.freedesktop.UDisks.Device"));

static const QString UDISKS_FIND_DEVICE(QString::fromAscii("FindDeviceByDeviceFile"));
static const QString UDISKS_MOUNT(QString::fromAscii("FilesystemMount"));
static const QString UDISKS_UNMOUNT(QString::fromAscii("FilesystemUnmount"));
#endif // QT_NO_UDISKS

class QDriveWatcherEngine : public QObject
{
    Q_OBJECT

public:
    explicit QDriveWatcherEngine(QObject *parent);
    ~QDriveWatcherEngine();

    inline bool isValid() const { return mtabWatchA > 0; }

Q_SIGNALS:
    void driveAdded(const QString &path);
    void driveRemoved(const QString &path);

private Q_SLOTS:
    void deviceChanged();
    void inotifyActivated();

private:
    QSet<QString> drives;
    int inotifyFD;
    int mtabWatchA;
};

static QSet<QString> getDrives()
{
    QSet<QString> result;

    FILE *fp = ::setmntent(_PATH_MOUNTED, "r");
    if (fp) {
        struct mntent *mnt;
        while ((mnt = ::getmntent(fp))) {
            QString rootPath = QFile::decodeName(mnt->mnt_dir);
            if (!result.contains(rootPath)) {
                result.insert(rootPath);
            }
        }
        ::endmntent(fp);
    }

    return result;
}

QDriveWatcherEngine::QDriveWatcherEngine(QObject *parent) :
    QObject(parent)
{
    drives = getDrives();
    inotifyFD = ::inotify_init();
    if (inotifyFD != -1) {
        mtabWatchA = ::inotify_add_watch(inotifyFD, _PATH_MOUNTED, IN_MODIFY);
        if (mtabWatchA > 0) {
            QSocketNotifier *notifier = new QSocketNotifier(inotifyFD, QSocketNotifier::Read/*, this*/);
            connect(notifier, SIGNAL(activated(int)), this, SLOT(inotifyActivated()));
        }
    }
}

QDriveWatcherEngine::~QDriveWatcherEngine()
{
    ::close(inotifyFD);
}

void QDriveWatcherEngine::deviceChanged()
{
    QSet<QString> allNewDrives = getDrives();

    foreach (const QString &drive, allNewDrives) {
        if (!drives.contains(drive))
            emit driveAdded(drive);
    }

    foreach (const QString &drive, drives) {
        if (!allNewDrives.contains(drive))
            emit driveRemoved(drive);
    }

    drives = allNewDrives;
}

void QDriveWatcherEngine::inotifyActivated()
{
    char buffer[1024];
    struct inotify_event *event;
    int len = ::read(inotifyFD, (void *)buffer, sizeof(buffer));
    if (len > 0) {
        event = (struct inotify_event *)buffer;
        if (event->wd == mtabWatchA /*&& (event->mask & IN_IGNORED) == 0*/) {
            ::inotify_rm_watch(inotifyFD, mtabWatchA);

            QTimer::singleShot(1000, this, SLOT(deviceChanged())); //give this time to finish write

            mtabWatchA = ::inotify_add_watch(inotifyFD, _PATH_MOUNTED, IN_MODIFY);
        }
    }
}

bool QDriveWatcher::start_sys()
{
    engine = new QDriveWatcherEngine(this);
    connect(engine, SIGNAL(driveAdded(QString)), this, SIGNAL(driveAdded(QString)));
    connect(engine, SIGNAL(driveRemoved(QString)), this, SIGNAL(driveRemoved(QString)));
    return engine->isValid();
}

void QDriveWatcher::stop_sys()
{
    delete engine;
    engine = 0;
}

static bool mountUdisks(const QString &device,
                         QString &mount_point,
                         const QString &fs,
                         const QStringList &options,
                         QDriveControllerPrivate::Error &error)
{
#if !defined(QT_NO_UDISKS)
    QDBusMessage findDevice =
            QDBusMessage::createMethodCall(UDISKS_SERVICE, UDISKS_PATH, UDISKS_INTERFACE, UDISKS_FIND_DEVICE);
    findDevice.setArguments(QVariantList() << device);

    QDBusReply<QDBusObjectPath> reply = QDBusConnection::systemBus().call(findDevice);
    if (reply.isValid()) {

        QDBusObjectPath devicePath = reply.value();
        QDBusInterface device(UDISKS_SERVICE, devicePath.path(), UDISKS_DEVICE_INTERFACE, QDBusConnection::systemBus());
        if (device.isValid()) {
            QDBusReply<QString> reply = device.call(UDISKS_MOUNT, fs, options);
            if (reply.isValid()) {
                mount_point = reply.value();
            } else {
                error.code = reply.error().type();
                error.string = reply.error().message();
                return false;
            }
        }

    } else {
        error.code = reply.error().type();
        error.string = reply.error().message();
        return false;
    }
    return true;
#else
    return false;
#endif
}

static bool unmountUdisks(const QString &device,
                           const QStringList &options,
                           QDriveControllerPrivate::Error &error)
{
#if !defined(QT_NO_UDISKS)
    QDBusMessage findDevice =
            QDBusMessage::createMethodCall(UDISKS_SERVICE, UDISKS_PATH, UDISKS_INTERFACE, UDISKS_FIND_DEVICE);
    findDevice.setArguments(QVariantList() << device);

    QDBusReply<QDBusObjectPath> reply = QDBusConnection::systemBus().call(findDevice);
    if (reply.isValid()) {

        QDBusObjectPath devicePath = reply.value();
        QDBusInterface device(UDISKS_SERVICE, devicePath.path(), UDISKS_DEVICE_INTERFACE, QDBusConnection::systemBus());
        if (device.isValid()) {
            QDBusReply<void> reply = device.call(UDISKS_UNMOUNT, options);
            if (!reply.isValid()) {
                error.code = reply.error().type();
                error.string = reply.error().message();
                return false;
            }
        }

    } else {
        error.code = reply.error().type();
        error.string = reply.error().message();
        return false;
    }
    return true;
#else
    return false;
#endif
}

bool QDriveController::mount(const QString &device, const QString &path)
{
    QString mountPath = path;
    return mountUdisks(device, mountPath, QString(), QStringList(), d->error);
}

bool QDriveController::unmount(const QString &path)
{
    return unmountUdisks(QDriveInfo(path).device(), QStringList(), d->error);
}

bool QDriveController::eject(const QString &device)
{
    if (!unmount(device))
        return false;

    int fd = ::open(QFile::encodeName(device), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        d->setError(errno);
        return false;
    }

    int result = 0;
#if defined(CDROMEJECT)
    result = ::ioctl(fd, CDROMEJECT);
#elif defined(CDIOCEJECT)
    result = ::ioctl(fd, CDIOCEJECT);
#endif
    if (result == -1) {
        d->setError(errno);
        close(fd);
        return false;
    }
    ::close(fd);
    return true;
}

#include "qdrivecontroller_linux.moc"
