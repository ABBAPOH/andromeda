#ifndef QDRIVECONTROLLER_H
#define QDRIVECONTROLLER_H

#include <QtCore/QObject>

#include "io_global.h"

class QDriveControllerPrivate;
class IO_EXPORT QDriveController : public QObject
{
    Q_OBJECT

public:
    explicit QDriveController(QObject *parent = 0);
    ~QDriveController();

    int error() const;
    QString errorString() const;

    bool mount(const QString &device, const QString &path = QString());
    bool unmount(const QString &path);
    bool eject(const QString &path);

Q_SIGNALS:
    void driveMounted(const QString &path);
    void driveUnmounted(const QString &path);

protected:
    QDriveControllerPrivate *const d;
};

#endif // QDRIVECONTROLLER_H
