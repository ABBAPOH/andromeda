#include "core.h"
#include "core_p.h"

#include <editormanager.h>
#include <pluginmanager.h>
#include <registrationmanager.h>

namespace Core {

Core *m_instance = 0;

Core::Core(QObject *parent) :
    ICore(parent),
    d_ptr(new CorePrivate)
{
    Q_ASSERT(m_instance);
    m_instance = this;
    setObjectName("Core");

    Q_D(Core);
    d->registrationManager = new RegistrationManager(this);
    d->registrationManager->setObjectName("RegistrationManager");

    d->editorManager = new EditorManager(this);
    d->editorManager->setObjectName("EditorManager");

connect(ExtensionSystem::PluginManager::instance(), SIGNAL(objectAdded(QObject*,QString)),
        d->registrationManager, SLOT(registerObject(QObject*,QString)));

}

Core::~Core()
{
    delete d_ptr;
    m_instance = 0;
}

ICore *ICore::instance()
{
    return m_instance;
}

EditorManager *Core::editorManager()
{
    return d_func()->editorManager;
}

RegistrationManager *Core::registrationManager()
{
    return d_func()->registrationManager;
}

} // namespace Core
