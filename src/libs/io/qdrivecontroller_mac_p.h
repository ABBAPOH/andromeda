#ifndef QDRIVECONTROLLER_MAC_P_H
#define QDRIVECONTROLLER_MAC_P_H

#include <QtCore/QSet>
#include <QtCore/QThread>

#include <DiskArbitration/DiskArbitration.h>

class QDriveWatcherEngine : public QThread
{
    Q_OBJECT

public:
    QDriveWatcherEngine();
    ~QDriveWatcherEngine();

    void stop();

    void addDrive(const QString &path);
    void removeDrive(const QString &path);
    void updateDrives();

protected:
    void run();

Q_SIGNALS:
    void driveAdded(const QString &path);
    void driveRemoved(const QString &path);

private:
    void populateVolumes();

    volatile bool m_running;

    DASessionRef m_session;
    QSet<QString> volumes;
};

#endif // QDRIVECONTROLLER_MAC_P_H
