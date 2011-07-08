#include "qdrivecontroller.h"
#include "qdrivecontroller_p.h"

QDriveWatcher::QDriveWatcher(QObject *parent)
    : QObject(parent),
      startStopCounter(0), engine(0)
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


QDriveController::QDriveController(QObject *parent)
    : QObject(parent), d(new QDriveControllerPrivate)
{
    if (QDriveWatcher *watcher = theWatcher()) {
        connect(watcher, SIGNAL(driveAdded(QString)),
                this, SIGNAL(driveMounted(QString)), Qt::QueuedConnection);
        connect(watcher, SIGNAL(driveRemoved(QString)),
                this, SIGNAL(driveUnmounted(QString)), Qt::QueuedConnection);

        watcher->start();
    }
}

QDriveController::~QDriveController()
{
    if (QDriveWatcher *watcher = theWatcher())
        watcher->stop();

    delete d;
}

int QDriveController::error() const
{
    return d->error.error();
}

QString QDriveController::errorString() const
{
    return d->error.toString();
}
