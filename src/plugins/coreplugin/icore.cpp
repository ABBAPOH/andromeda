#include "icore.h"

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

