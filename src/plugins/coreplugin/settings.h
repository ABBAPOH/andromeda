#ifndef SETTINGS_H
#define SETTINGS_H

#include "coreplugin_global.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace CorePlugin {

class SettingsPrivate;
class COREPLUGIN_EXPORT Settings : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Settings)
    Q_DISABLE_COPY(Settings)

public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    void clear();
    void sync();

    void beginGroup(const QString &prefix);
    void endGroup();
    QString group() const;

    int beginReadArray(const QString &prefix);
    void beginWriteArray(const QString &prefix, int size = -1);
    void endArray();
    void setArrayIndex(int i);

    QStringList allKeys() const;
    QStringList childKeys() const;
    QStringList childGroups() const;

    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);

    void remove(const QString &key);
    bool contains(const QString &key) const;

    void addObject(QObject *o, const QString &key);
    void removeObject(QObject *o, const QString &key);
    void removeObject(QObject *o);

signals:
    void valueChanged(const QString &key, const QVariant &value);

private slots:
    void onDestroy(QObject *o);

protected:
    SettingsPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // SETTINGS_H
