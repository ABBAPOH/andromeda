#include "settings.h"
#include "settings_p.h"

#include <QtCore/QSettings>
#include <QtCore/QStringList>

using namespace CorePlugin;

static QByteArray keyToProperty(const QString &key)
{
    int index = key.lastIndexOf(QLatin1Char('/'));
    return key.mid(index + 1).toLatin1();
}

PropertyMonitor::PropertyMonitor(QObject *parent) :
    QObject(parent)
{
}

void PropertyMonitor::onPropertyChanged()
{
    QObject *o = sender();
    QVariant value = o->property(keyToProperty(key()).constData());
    emit updateValue(key(), value);
}

QString SettingsPrivate::longKey(const QString &key) const
{
    QString group = settings->group();
    return group.isEmpty() ? key : (group + QLatin1Char('/') + key);
}

Settings::Settings(QObject *parent) :
    QObject(parent),
    d_ptr(new SettingsPrivate)
{
    Q_D(Settings);

    d->settings = new QSettings(this);
}

Settings::~Settings()
{
    delete d_ptr;
}

void Settings::clear()
{
    Q_D(Settings);

    d->settings->clear();
}

void Settings::sync()
{
    Q_D(Settings);

    d->settings->sync();
}

void Settings::beginGroup(const QString &prefix)
{
    Q_D(Settings);

    d->settings->beginGroup(prefix);
}

void Settings::endGroup()
{
    Q_D(Settings);

    d->settings->endGroup();
}

QString Settings::group() const
{
    Q_D(const Settings);

    return d->settings->group();
}

int Settings::beginReadArray(const QString &prefix)
{
    Q_D(Settings);

    return d->settings->beginReadArray(prefix);
}

void Settings::beginWriteArray(const QString &prefix, int size)
{
    Q_D(Settings);

    d->settings->beginWriteArray(prefix, size);
}

void Settings::endArray()
{
    Q_D(Settings);

    d->settings->endArray();
}

void Settings::setArrayIndex(int i)
{
    Q_D(Settings);

    d->settings->setArrayIndex(i);
}

QStringList Settings::allKeys() const
{
    Q_D(const Settings);

    return d->settings->allKeys();
}

QStringList Settings::childKeys() const
{
    Q_D(const Settings);

    return d->settings->childKeys();
}

QStringList Settings::childGroups() const
{
    Q_D(const Settings);

    return d->settings->childGroups();
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    Q_D(Settings);

    d->settings->setValue(key, value);

    emit valueChanged(key, value);

    QString longKey = d->longKey(key);

    foreach (QObject* o, d->hash.values(longKey)) {
        o->setProperty(keyToProperty(longKey), value);
    }
}

QVariant Settings::value(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const Settings);

    return d->settings->value(key, defaultValue);
}

void Settings::remove(const QString &key)
{
    Q_D(Settings);

    d->settings->remove(key);
}

bool Settings::contains(const QString &key) const
{
    Q_D(const Settings);

    return d->settings->contains(key);
}

#include <QMetaProperty>
void Settings::addObject(QObject *o, const QString &key)
{
    Q_D(Settings);

    QVariant value = d->settings->value(key);
    if (value.isValid()) {
#ifdef Q_OS_LINUX
        // fucken linux
        if (value.canConvert(QVariant::Int))
            value = value.toInt();
#endif
        o->setProperty(keyToProperty(key), value);
    }

    d->hash.insert(d->longKey(key), o);
    connect(o, SIGNAL(destroyed(QObject*)), SLOT(onDestroy(QObject*)));

    QByteArray propertyName = keyToProperty(key);
    PropertyMonitor *monitor = d->monitors.value(d->longKey(key));
    if (!monitor) {
        monitor = new PropertyMonitor(this);
        monitor->setKey(d->longKey(key));
        d->monitors.insert(d->longKey(key), monitor);
        connect(monitor, SIGNAL(updateValue(QString,QVariant)),
                SLOT(updateProperty(QString,QVariant)));
    }

    const QMetaObject *mo = o->metaObject();
    QMetaProperty property = mo->property(mo->indexOfProperty(propertyName));
    QMetaMethod signal = property.notifySignal();
    if (signal.methodIndex() != -1)
        connect(o, QByteArray("2") + signal.signature(), monitor, SLOT(onPropertyChanged()));
}

void Settings::removeObject(QObject *o, const QString &key)
{
    Q_D(Settings);

    d->hash.remove(key, o);
}

void Settings::removeObject(QObject *o)
{
    Q_D(Settings);

    QStringList keys = d->hash.keys(o);
    foreach (const QString key, keys) {
        d->hash.remove(key, o);
    }
}

void Settings::updateProperty(const QString &key, const QVariant &value)
{
    Q_D(Settings);

    d->settings->setValue(key, value);
}

void Settings::onDestroy(QObject *o)
{
    removeObject(o);
}
