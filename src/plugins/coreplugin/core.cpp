#include "core.h"
#include "core_p.h"

#include <pluginmanager.h>

#include "editormanager.h"
#include "settings.h"

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

Core::Core(QObject *parent) :
    QObject(parent),
    d_ptr(new CorePrivate)
{
    Q_D(Core);
    d->pool = ExtensionSystem::PluginManager::instance();
    setObjectName("core");
}

Core::~Core()
{
    delete d_ptr;
}

QObject *Core::getObject(const QString &name)
{
    Q_D(Core);
    return d->pool->object(name);
}

EditorManager * Core::editorManager() const
{
    return d_func()->pool->object<EditorManager>("editorManager");
}

Settings * Core::settings() const
{
    return d_func()->pool->object<Settings>("settings");
}
