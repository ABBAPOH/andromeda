#include "core.h"
#include "core_p.h"

#include <editormanager.h>
#include <pluginmanager.h>
#include <registrationmanager.h>

/*!
    \namespace Core
    \brief This namespace contains main classes for Andromeda application.
*/

/*!
    \class Core::ICore
    \brief Main interface for Andromeda app.

    Provide access for main classes in Andromeda's core.
    Core is created during initialization of core plugin.
    Use ICore::instance() method to get an instance of an object.
    All classes that can be got via ICore methods can also be reached by name:
    ICore::instance()->registrationManager()
    is equal to
    ICore::instance()->findChild("RegistrationManager")
    Note that the first letter is capital, because objects are name by their type.
    This feature can be useful when working with Core via QtScript.
*/

/*!
    \fn ICore::ICore(QObject *parent)
    \internal
*/

/*!
    \fn ICore::~ICore()
    \internal
*/

/*!
    \fn ICore *ICore::instance();
    \brief Returns an instance of a core.
    This method can be used only when Core::Core object will be created.
*/

/*!
    \fn RegistrationManager *ICore::registrationManager()
    \brief Returns a RegistrationManager object.
*/

using namespace CorePlugin;

Core *m_instance = 0;

Core::Core(QObject *parent) :
    QObject(parent),
    d_ptr(new CorePrivate)
{
    Q_ASSERT(!m_instance);
    m_instance = this;
    setObjectName("Core");

    Q_D(Core);
    d->registrationManager = new RegistrationSystem::RegistrationManager(this);
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

Core *Core::instance()
{
    return m_instance;
}

EditorManager *Core::editorManager()
{
    return d_func()->editorManager;
}

RegistrationSystem::RegistrationManager *Core::registrationManager()
{
    return d_func()->registrationManager;
}
