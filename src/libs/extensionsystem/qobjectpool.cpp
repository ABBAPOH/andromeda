#include "qobjectpool.h"
#include "qobjectpool_p.h"

QObjectPoolPrivate::QObjectPoolPrivate()
{
}

QObjectPoolPrivate::~QObjectPoolPrivate()
{
}

QObjectPool::QObjectPool(QObject *parent) :
    QObject(parent),
    d_ptr(new QObjectPoolPrivate)
{
}

QObjectPool::QObjectPool(QObjectPoolPrivate &dd, QObject* parent) :
        QObject(parent),
        d_ptr(&dd)
{
}

QObjectPool::~QObjectPool()
{
    delete d_ptr;
}

/*!
    \fn void QObjectPool::addObject(QObject * object, const QString &type)
    \brief Adds \a object to object pool.

    There are 3 kinds of objects - named objects, typed objects and other objects.
    If object name if not an empty string, object counts as named object.
    If \a type if not an empty string, object counts as typed object.

    In fact, there can be many objects with same name.
*/
void QObjectPool::addObject(QObject * object, const QString &type)
{
    Q_D(QObjectPool);
    if (!object) {
        return;
    }

    if (!d->objects.contains(object)) {
        d->objects.append(object);
        if (object->objectName() != "") {
            d->namedObjects.insertMulti(object->objectName(), object);
        }
        emit objectAdded(object, type);
    }
}

/*!
    \fn void QObjectPool::removeObject(QObject * object)
    \brief Removes \a object to object pool.

    \sa QObjectPool::addObject
*/
void QObjectPool::removeObject(QObject * object)
{
    Q_D(QObjectPool);

    if (!object) {
        return;
    }

    d->objects.removeAll(object);
    emit objectRemoved(object);
}

QObjectList QObjectPool::objects()
{
    return d_func()->objects;
}

/*!
    \fn QObject * QObjectPool::object(const QString &name)
    \brief Returns object from object pool with objectName property equal to \a name.

    \sa QObjectPool::addObject
*/
QObject * QObjectPool::object(const QString &name)
{
    return d_func()->namedObjects.value(name);
}

/*!
    \fn QObject * QObjectPool::objects(const QString &name)
    \brief Returns list object from object pool with objectName property equal to \a name.

    \sa PluginManager::addObject
*/
QObjectList QObjectPool::objects(const QString &name)
{
    return d_func()->namedObjects.values(name);
}
