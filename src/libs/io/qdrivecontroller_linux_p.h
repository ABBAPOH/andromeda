#ifndef QDRIVECONTROLLER_LINUX_P_H
#define QDRIVECONTROLLER_LINUX_P_H

#include <QtCore/QSet>

class QDriveWatcherEngine : public QObject
{
    Q_OBJECT

public:
    QDriveWatcherEngine(QObject *parent);
    ~QDriveWatcherEngine();

    inline bool isValid() const
    { return mtabWatchA > 0; }

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

#endif // QDRIVECONTROLLER_LINUX_P_H
