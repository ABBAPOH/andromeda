#ifndef QDRIVECONTROLLER_P_H
#define QDRIVECONTROLLER_P_H

#include "qdrivecontroller.h"

#include <QtCore/QAtomicInt>
#include <QtCore/QObject>

#include "qsystemerror_p.h"

class QDriveControllerPrivate
{
public:
    struct Error
    {
        int code;
        QString string;
    };

    inline void setError(int errorCode, const QString& errorString)
    {
        error.code = errorCode;
        error.string = errorString;
    }

    inline void setError(int errorCode)
    {
        QSystemError systemError(errorCode, QSystemError::NativeError);
        error.code = systemError.errorCode;
        error.string = systemError.toString();
    }

    inline void setError(Error error)
    {
        error = error;
    }

    Error error;
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

#endif // QDRIVECONTROLLER_P_H
