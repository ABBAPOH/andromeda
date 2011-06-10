#include "filemanagerpluginimpl.h"

#include <QtCore/QtPlugin>
#include <perspective.h>
#include <guicontroller.h>
#include <CorePlugin>

#include "filemanagerview.h"
#include "navigationpanelview.h"
#include "filecopydialog.h"
#include "filesystemmanager.h"

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
    perspective->addView("NavigationPanel", 0);
    perspective->addView("FileManager", 4);
    perspective->setProperty("MainView", "FileManager");
    GuiController::instance()->addPerspective(perspective);

    CorePlugin::Core::instance()->perspectiveManager()->addPerspective("inode/directory",
                                                                       "FileManagerPerspective");

    addObject(new FileCopyDialog, "fileCopyDialog");
    addObject(FileSystemManager::instance());

    return true;
}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)
