#ifndef QDRIVECONTROLLER_P_H
#define QDRIVECONTROLLER_P_H

#include "qdrivecontroller.h"

#include <QtCore/QAtomicInt>
#include <QtCore/QObject>

#include "qsystemerror_p.h"

class QDriveControllerPrivate
{
public:
    inline void setLastError(int errorCode)
    {
        error.errorScope = QSystemError::NativeError;
        error.errorCode = errorCode;
    }

    QSystemError error;
};


class QDriveWatcherEngine;

class QDriveWatcher : public QObject
{
    Q_OBJECT

public:
    explicit QDriveWatcher(QObject *parent = 0);
    ~QDriveWatcher();

    void start();
    void stop();

    inline void emitDriveAdded(const QString &driveOrPath)
    { emit driveAdded(driveOrPath); }
    inline void emitDriveRemoved(const QString &driveOrPath)
    { emit driveRemoved(driveOrPath); }

Q_SIGNALS:
    void driveAdded(const QString &path);
    void driveRemoved(const QString &path);

protected:
    bool start_sys();
    void stop_sys();

private:
    QAtomicInt startStopCounter;
    QDriveWatcherEngine *engine;
};


#if defined(Q_OS_MAC)

#include "qdrivecontroller_mac_p.h"

#elif defined(Q_OS_LINUX)

#include "qdrivecontroller_linux_p.h"

#endif

#endif // QDRIVECONTROLLER_P_H
