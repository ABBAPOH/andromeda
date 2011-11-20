#ifndef SETTINGS_P_H
#define SETTINGS_P_H

#include "settings.h"

class QSettings;

namespace CorePlugin {

class PropertyMonitor :  public QObject
{
    Q_OBJECT

public:
    explicit PropertyMonitor(QObject *parent = 0);

    inline QString key() const { return m_key; }
    inline void setKey(const QString &name) { m_key = name; }

private slots:
    void onPropertyChanged();

signals:
    void updateValue(const QString &key, const QVariant &value);

private:
    QString m_key;
};

class SettingsPrivate
{
public:
    QSettings *settings;
    QMultiHash< QString, QObject * > hash;
    QHash< QString, PropertyMonitor * > monitors;

    QString longKey(const QString &key) const;
};

} // namespace CorePlugin

#endif // SETTINGS_P_H
