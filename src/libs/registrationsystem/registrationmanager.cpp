#include "registrationmanager.h"

#include <QtCore/QStringList>
#include <QtCore/QMultiHash>

/*!
    \namespace RegistrationSystem
    \brief The RegistrationSystem namespace contains only one class - RegistrationSystem::RegistrationManager.
    This class is used to improve speed of registration process.
*/

/*!
    \class RegistrationSystem::RegistrationManager
    \brief This class is used to improve speed of registration process.

    Objects that added to ExtensionSystem::PluginManager pool can be handled directly by
    connecting to ExtensionSystem::PluginManager::objectAdded signal. However, this class
    can improve speed of this process. To register an object of specific type, you need to
    implement RegistrationSystem::IRegistrator interface and add it to system via
    RegistrationSystem::RegistrationManager::addRegistrator method.
*/

/*!
    \fn void RegistrationManager::addRegistrator(IRegistrator *registrator, const QString &type)
    \brief This function adds registrator to a registration system.

    One registrator can be added multiple times using different types.

    \a type parameter is used to determine objects of what type we want to register
    IRegistrator::registerObject function will only be called if type of object added
    to pool equal to type of registrator.
*/

/*!
    \fn void RegistrationManager::removeRegistrator(IRegistrator *registrator)
    \brief Removes ALL instances of \a registrator from system.
*/

/*!
    \interface RegistrationSystem::IRegistrator
    \brief This interface is a base interface for all registrators.

    To register any object, you have to reimplement at least
    RegistrationSystem::IRegistrator::registerObject method. After that, you need to
    add instance of this class to instance of RegistrationSystem::RegistrationManager.
*/

/*!
    \fn bool IRegistrator::canRegister(QObject *object)
    \brief Can be used to provide checks if object is valid and it's type
    is really what we want to see.

    Default implementation returns true.
*/

/*!
    \fn void objectRegistered(QObject *object)
    \brief Emited when \a object successfully registered.
*/

/*!
    \fn bool IRegistrator::registerObject(QObject *object, const QString &type)
    \brief This function should do all custom actions to register an object.

    It is automatically called when object added to ExtensionSystem::PluginManager pool.
*/

namespace RegistrationSystem {

// ============= RegistrationManagerPrivate =============

class RegistrationManagerPrivate
{
public:
    QMultiHash<QString, IRegistrator *> registrators;
};

} // namespace RegistrationSystem

using namespace RegistrationSystem;

// ============= RegistrationManager =============

RegistrationManager::RegistrationManager(QObject *parent) :
    QObject(parent),
    d_ptr(new RegistrationManagerPrivate())
{
}

RegistrationManager::~RegistrationManager()
{
    delete d_ptr;
}

void RegistrationManager::addRegistrator(IRegistrator *registrator, const QString &type)
{
    Q_D(RegistrationManager);
    if (!d->registrators.values(type).contains(registrator)) {
        d->registrators.insert(type, registrator);
    }
}

void RegistrationManager::removeRegistrator(IRegistrator *registrator)
{
    Q_D(RegistrationManager);

    QStringList keys = d->registrators.keys(registrator);
    foreach (QString key, keys) {
        d->registrators.remove(key);
    }
}

void RegistrationManager::registerObject(QObject *object, const QString &type)
{
    Q_D(RegistrationManager);
    if (type == "") {
        foreach (IRegistrator *registrator, d->registrators.values(type)) {
            if (registrator->canRegister(object)) {
                bool result = registrator->registerObject(object);
                if (result)
                    emit objectRegistered(object);
            }
        }
    } else {
        foreach (IRegistrator *registrator, d->registrators.values(type)) {
            bool result = registrator->registerObject(object);
            if (result)
                emit objectRegistered(object);
        }
    }
}
