#ifndef SHAREDPROPERTIES_P_H
#define SHAREDPROPERTIES_P_H

#include "sharedproperties.h"

#include <QtCore/QMutex>
#include <QtCore/QStringList>
#include <QtCore/QEvent>

namespace GuiSystem {

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

    void setDefaultValue(const QString &key, const QVariant &value);
    void notifyValueChanged(const QString &key, const QVariant &value);

public:
    QVariantMap values;
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

} //namespace GuiSystem

#endif // SHAREDPROPERTIES_P_H
