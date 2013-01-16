#include "toolwidgetmanager.h"

#include <QtCore/QDebug>
#include <QtCore/QMap>

#include "toolwidgetfactory.h"

namespace GuiSystem {

class ToolWidgetManagerPrivate
{
public:
    QMap<QByteArray, ToolWidgetFactory *> factories;
};

} // namespace GuiSystem

using namespace GuiSystem;

/*!
    \class ToolWidgetManager

    ToolWidgetManager is a container for ToolWidgetFactories, it allows to
    register factories and receive them by id.
*/

/*!
    Creates ToolWidgetManager with the given \a parent.
*/
ToolWidgetManager::ToolWidgetManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ToolWidgetManagerPrivate)
{
}

/*!
    Destroys ToolWidgetManager.
*/
ToolWidgetManager::~ToolWidgetManager()
{
    delete d_ptr;
}

Q_GLOBAL_STATIC(ToolWidgetManager, staticInstance)

// TODO: think if it possible to have only one singletone (aka PartManager)
/*!
    Returns static instance of a ToolWidgetManager.
*/
ToolWidgetManager * ToolWidgetManager::instance()
{
    return staticInstance();
}

/*!
    Returns all factories added to the ToolWidgetManager.
*/
QList<ToolWidgetFactory *> ToolWidgetManager::factories() const
{
    Q_D(const ToolWidgetManager);
    return d->factories.values();
}

/*!
    Returns factory with \a id, or 0 if no factory found with this id.
*/
ToolWidgetFactory * ToolWidgetManager::factory(const QByteArray &id) const
{
    Q_D(const ToolWidgetManager);
    return d->factories.value(id);
}

/*!
    Registers \a factory in the ToolWidgetManager.
*/
void ToolWidgetManager::addFactory(ToolWidgetFactory *factory)
{
    Q_D(ToolWidgetManager);

    if (!factory)
        return;

    const QByteArray id =  factory->id();
    if (d->factories.contains(id))
        qWarning() << "ToolWidgetManager::addFatory"
                   << "factory with id" << id << "already added.";

    d->factories.insert(id, factory);
    connect(factory, SIGNAL(destroyed(QObject*)), SLOT(onFactoryDestroyed(QObject*)));
}

/*!
    Unregisters \a factory from the ToolWidgetManager.
*/
void ToolWidgetManager::removeFactory(ToolWidgetFactory *factory)
{
    Q_D(ToolWidgetManager);

    if (!factory)
        return;

    QByteArray id = d->factories.key(factory);
    d->factories.remove(id);
}

/*!
    \internal
*/
void ToolWidgetManager::onFactoryDestroyed(QObject *object)
{
    removeFactory(static_cast<ToolWidgetFactory*>(object));
}
