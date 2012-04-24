#ifndef KDESETTINGS_H
#define KDESETTINGS_H

#include <QSettings>

class KDESettings : public QSettings
{
    Q_OBJECT
    Q_DISABLE_COPY(KDESettings)

public:
    explicit KDESettings(QObject *parent = 0);
    KDESettings(const QString &fileName, QObject *parent = 0);

    static QSettings::Format KDEIniFormat;
};

#endif // KDESETTINGS_H
