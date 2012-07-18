#include "qobjectpool.h"
#include "qobjectpool_p.h"

using namespace ExtensionSystem;

/*!
    \class ExtensionSystem::QObjectPool

    \brief QObjectPool class represents collection of QObjects.

    QObjectPool allows to retrieve all objects or a single object by name.

    \sa addObject(), object(), objects()
*/

/*!
    Constructs QObjectPool with the given \a parent;
*/
QObjectPool::QObjectPool(QObject *parent) :
    QObject(parent),
    d_ptr(new QObjectPoolPrivate)
{
}

/*!
    \internal
*/
QObjectPool::QObjectPool(QObjectPoolPrivate &dd, QObject* parent) :
        QObject(parent),
        d_ptr(&dd)
{
}

/*!
    Destroys QObjectPool.
*/
QObjectPool::~QObjectPool()
{
    delete d_ptr;
}

/*!
    \brief Adds \a object to object pool.

    There are 2 kinds of objects - named objects, and other objects.
    You can retrieve named objects using object() or objects() function (it is
    possible to add multiple objects with the same name)

    If object name is empty, and \a name parameter is not, \a object's name
    is set to \a name. If both are empty, \a object is counted as unnamed
    object and simply added to pool.
*/
void QObjectPool::addObject(QObject * object, const QString &name)
{
    Q_D(QObjectPool);
    if (!object) {
        return;
    }

    if (!name.isEmpty() || name == QLatin1String(object->metaObject()->className())) {
        object->setObjectName(name);
    }

    if (!d->objects.contains(object)) {
        d->objects.append(object);
        if (object->objectName() != "") {
            d->namedObjects.insertMulti(object->objectName(), object);
        }
        emit objectAdded(object);
    }
}

/*!
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

/*!
    \brief Returns list of all objects contained in the object pool.

    \sa QObjectPool::addObject
*/
QObjectList QObjectPool::objects() const
{
    return d_func()->objects;
}

/*!
    \brief Returns object from object pool with objectName property equal to \a name.

    \sa QObjectPool::addObject
*/
QObject * QObjectPool::object(const QString &name) const
{
    return d_func()->namedObjects.value(name);
}

/*!
    \brief Returns list of objects from object pool with objectName property equal to \a name.

    \sa PluginManager::addObject
*/
QObjectList QObjectPool::objects(const QString &name) const
{
    return d_func()->namedObjects.values(name);
}
