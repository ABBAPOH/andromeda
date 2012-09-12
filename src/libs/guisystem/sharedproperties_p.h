#ifndef SHAREDPROPERTIES_P_H
#define SHAREDPROPERTIES_P_H

#include "sharedproperties.h"

#include <QtCore/QMutex>
#include <QtCore/QStringList>
#include <QtCore/QEvent>

namespace GuiSystem {

class StaticSharedProperties
{
public:
    StaticSharedProperties();

    static StaticSharedProperties *instance();

    void addProperites(SharedPropertiesPrivate *properties);
    void removeProperites(SharedPropertiesPrivate *properties);

    QVariant value(const QString &key, const QVariant &defaulValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);
    void setDefaultValue(const QString &key, const QVariant &value);

    void notifyValueChanged(const QString &key, const QVariant &value);

public:
    mutable QMutex mutex;
    QList<SharedPropertiesPrivate*> objects;
    QVariantMap values;
};

class SharedPropertiesPrivate
{
    Q_DECLARE_PUBLIC(SharedProperties)
    Q_DISABLE_COPY(SharedPropertiesPrivate)
public:
    struct Key
    {
        Key() : object(0), id(-1) {}
        Key (QObject *o, int i) : object(o), id(i) {}

        QObject *object;
        int id;

        bool operator == (const Key &other) const;
        bool operator < (const Key &other) const;
    };
    typedef Key Property;
    typedef Key Notifier;
    SharedPropertiesPrivate(SharedProperties *qq);

    void notifyValueChanged(const QString &key, const QVariant &value);

public:
    StaticSharedProperties *staticProperties;
    QString group;
    QStringList groupStack;
    QMultiMap<QString, Property> mapKeyToProperty;
    QMap<Notifier, QString> mapNotifierToKey;

private:
    SharedProperties *q_ptr;

    friend class StaticSharedProperties;
};

bool SharedPropertiesPrivate::Key::operator ==(const Key &other) const
{
    return object == other.object && id == other.id;
}

bool SharedPropertiesPrivate::Key::operator <(const Key &other) const
{
    if (object == other.object)
        return id < other.id;

    return object < other.object;
}

class SharedPropertiesEvent : public QEvent
{
public:
    SharedPropertiesEvent(const QString &key, const QVariant &value);

    QString key() const { return m_key; }
    QVariant value() const { return m_value; }

private:
    QString m_key;
    QVariant m_value;
};

} //namespace GuiSystem

#endif // SHAREDPROPERTIES_P_H
