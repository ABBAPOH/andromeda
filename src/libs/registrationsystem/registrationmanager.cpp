#include "registrationmanager.h"

#include <QtCore/QStringList>
#include <QtCore/QMultiHash>

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
