#include "sharedproperties.h"
#include "sharedproperties_p.h"

#include <QtCore/QDebug>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>
#include <QtCore/QThread>

#include <QtGui/QApplication>

using namespace GuiSystem;

typedef SharedPropertiesPrivate::Property Property;
typedef SharedPropertiesPrivate::Notifier Notifier;

static QEvent::Type sharedPropertiesEventType = QEvent::None;
static const char * const OnValueChangedSlot = "onValueChanged()";

static QString shortKey(const QString &longKey)
{
    int index = longKey.lastIndexOf('/');

    return longKey.mid(index + 1);
}

static QString longKey(const QString &group, const QString &shortKey)
{
    if (group.isEmpty())
        return shortKey;

    return group + '/' + shortKey;
}

StaticSharedProperties::StaticSharedProperties()
{
    ::sharedPropertiesEventType = (QEvent::Type)QEvent::registerEventType();
}

Q_GLOBAL_STATIC(StaticSharedProperties, static_instance)
StaticSharedProperties * StaticSharedProperties::instance()
{
    return static_instance();
}

void StaticSharedProperties::addProperites(SharedPropertiesPrivate *properties)
{
    QMutexLocker l(&mutex);
    objects.append(properties);
}

void StaticSharedProperties::removeProperites(SharedPropertiesPrivate *properties)
{
    QMutexLocker l(&mutex);
    objects.removeOne(properties);
}

QVariant StaticSharedProperties::value(const QString &key, const QVariant &defaulValue) const
{
    QMutexLocker l(&mutex);
    return values.value(key, defaulValue);
}

void StaticSharedProperties::setValue(const QString &key, const QVariant &value)
{
    bool changed = false;
    {
        QMutexLocker l(&mutex);
        if (values.value(key) != value) {
            values.insert(key, value);
            changed = true;
        }
    }
    if (changed)
        notifyValueChanged(key, value);
}

void StaticSharedProperties::setDefaultValue(const QString &key, const QVariant &value)
{
    QMutexLocker l(&mutex);
    values.insert(key, value);
}

void StaticSharedProperties::notifyValueChanged(const QString &key, const QVariant &value)
{
    QThread *currentThread = QThread::currentThread();

    foreach (SharedPropertiesPrivate *d, objects) {
        if (d->q_func()->thread() == currentThread)
            d->notifyValueChanged(key, value);
        else
            qApp->postEvent(d->q_func(), new SharedPropertiesEvent(key, value));
    }
}

SharedPropertiesPrivate::SharedPropertiesPrivate(SharedProperties *qq) :
    q_ptr(qq)
{
}

void SharedPropertiesPrivate::notifyValueChanged(const QString &key, const QVariant &value)
{
    foreach (const Property &prop, mapKeyToProperty.values(key)) {
        const QMetaObject *metaObject = prop.object->metaObject();
        QMetaProperty property = metaObject->property(prop.id);
        property.write(prop.object, value);
    }
}

SharedPropertiesEvent::SharedPropertiesEvent(const QString &key, const QVariant &value) :
    QEvent(::sharedPropertiesEventType),
    m_key(key),
    m_value(value)
{
}

SharedProperties::SharedProperties(QObject *parent) :
    QObject(parent),
    d_ptr(new SharedPropertiesPrivate(this))
{
    Q_D(SharedProperties);
    d->staticProperties = StaticSharedProperties::instance();
    d->staticProperties->addProperites(d);
}

SharedProperties::~SharedProperties()
{
    Q_D(SharedProperties);
    d->staticProperties->removeProperites(d);
    delete d_ptr;
}

bool SharedProperties::addObject(const QString &key, QObject *object)
{
    return addObject(key, object, shortKey(key).toLatin1());
}

bool SharedProperties::addObject(const QString &key, QObject *object, const QByteArray &propertyName)
{
    Q_D(SharedProperties);

    QString longKey = ::longKey(d->group, key);

    const QMetaObject *metaObject = object->metaObject();
    int propertyId = metaObject->indexOfProperty(propertyName.constData());

    if (propertyId == -1) {
        qWarning() << "SharedProperties::addObject :" << "Object" << object << "doesn't have property" << propertyName;
        return false;
    }

    Property propertyKey(object, propertyId);
    d->mapKeyToProperty.insert(longKey, propertyKey);

    QMetaProperty metaProperty = metaObject->property(propertyId);
    metaProperty.write(object, value(longKey));

    int notifierId = metaProperty.notifySignalIndex();
    if (notifierId == -1) {
        qWarning() << "SharedProperties::addObject :" << "Property" << propertyName << "doesn't have notifier signal";
        return false;
    }

    Notifier notifierKey(object, notifierId);
    d->mapNotifierToKey.insert(notifierKey, longKey);
    QMetaMethod notifierMethod = metaObject->method(notifierId);

    const QMetaObject *thisMetaObject = this->metaObject();
    int handlerId = thisMetaObject->indexOfSlot(OnValueChangedSlot);
    QMetaMethod handlerMethod = thisMetaObject->method(handlerId);

    connect(object, notifierMethod, this, handlerMethod);

    return true;
}

void SharedProperties::removeObject(QObject *object)
{
    Q_D(SharedProperties);

    QMutableMapIterator<QString, Property> it1(d->mapKeyToProperty);
    while (it1.hasNext()) {
        it1.next();
        if (it1.value().object == object)
            it1.remove();
    }

    QMutableMapIterator<Notifier, QString> it2(d->mapNotifierToKey);
    while (it2.hasNext()) {
        it2.next();
        if (it2.key().object == object)
            it2.remove();
    }
}

QVariant SharedProperties::value(const QString &key, const QVariant &defaulValue) const
{
    Q_D(const SharedProperties);

    QString longKey = ::longKey(d->group, key);
    return d->staticProperties->value(longKey, defaulValue);
}

void SharedProperties::setValue(const QString &key, const QVariant &value)
{
    Q_D(SharedProperties);

    QString longKey = ::longKey(d->group, key);
    d->staticProperties->setValue(longKey, value);
}

QString SharedProperties::group() const
{
    Q_D(const SharedProperties);

    return d->group;
}

void SharedProperties::beginGroup(const QString &group)
{
    Q_D(SharedProperties);

    d->groupStack.append(group);
    d->group = d->groupStack.join("/");
}

void SharedProperties::endGroup()
{
    Q_D(SharedProperties);

    if (d->groupStack.isEmpty()) {
        qWarning() << "Not symmetrical calls to SharedProperties::beginGroup()/endGroup()";
        return;
    }

    d->groupStack.takeLast();
    d->group = d->groupStack.join("/");
}

void SharedProperties::onDestroyed(QObject *object)
{
    removeObject(object);
}

void SharedProperties::onValueChanged()
{
    Q_D(SharedProperties);

    QObject *object = sender();
    const QMetaObject *metaObject = object->metaObject();
    int notifierId = senderSignalIndex();
    Notifier notifierKey(object, notifierId);
    QString longKey = d->mapNotifierToKey.value(notifierKey);

    Property propertyKey;
    foreach (propertyKey, d->mapKeyToProperty.values(longKey)) {
        if (propertyKey.object == object)
            break;
    }

    QMetaProperty metaProperty = metaObject->property(propertyKey.id);
    QVariant value = metaProperty.read(object);

    d->staticProperties->setValue(longKey, value);
}

bool SharedProperties::event(QEvent *event)
{
    if (event->type() == ::sharedPropertiesEventType) {
        Q_D(SharedProperties);
        SharedPropertiesEvent *spe = static_cast<SharedPropertiesEvent*>(event);
        d->notifyValueChanged(spe->key(), spe->value());
        return true;
    }

    return QObject::event(event);
}
