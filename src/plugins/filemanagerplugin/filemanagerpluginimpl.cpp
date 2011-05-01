#include "filemanagerpluginimpl.h"

#include <QtCore/QtPlugin>
#include <perspective.h>
#include <guicontroller.h>
#include <CorePlugin>

#include "filemanagerview.h"
#include "navigationpanelview.h"

using namespace FileManagerPlugin;
using namespace ExtensionSystem;
using namespace GuiSystem;

FileManagerPluginImpl::FileManagerPluginImpl(QObject *parent) :
    ExtensionSystem::IPlugin()
{
}

bool FileManagerPluginImpl::initialize()
{
    GuiController::instance()->addFactory(new FileManagerFactory);
    GuiController::instance()->addFactory(new NavigationPanelFactory);

    Perspective *perspective = new Perspective("FileManagerPerspective");
    perspective->addView("FileManager", 6);
    perspective->addView("NavigationPanel", 1);
    perspective->setProperty("MainView", "FileManager");
    GuiController::instance()->addPerspective(perspective);

    CorePlugin::Core::instance()->perspectiveManager()->addPerspective("inode/directory",
                                                                       "FileManagerPerspective");

    return true;
}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)
