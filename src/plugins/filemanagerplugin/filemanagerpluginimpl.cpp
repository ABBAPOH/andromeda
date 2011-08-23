#include "filemanagerpluginimpl.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDir>

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <perspective.h>
#include <guicontroller.h>
#include <CorePlugin>

#include "filecopydialog.h"
#include "filemanagerview.h"
#include "filesystemmanager.h"
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
    perspective->addView("NavigationPanel", 0);
    perspective->addView("FileManager", 4);
    perspective->setProperty("MainView", "FileManager");
    GuiController::instance()->addPerspective(perspective);

    CorePlugin::Core::instance()->perspectiveManager()->addPerspective("inode/directory",
                                                                       "FileManagerPerspective");

    addObject(new FileCopyDialog, "fileCopyDialog");
    addObject(FileSystemManager::instance());

    CommandContainer *goToContainer = ActionManager::instance()->container(Constants::Ids::Menus::GoTo);
    // ================ GoTo Menu (Locations) ================
    goToContainer->addGroup(Constants::Ids::MenuGroups::Locations);

    createGotoDirCommand(QDesktopServices::DesktopLocation, QIcon(":/images/icons/desktopFolder.png"), tr("Ctrl+Shift+D"));
    createGotoDirCommand(QDesktopServices::HomeLocation,
                         QIcon::fromTheme("go-home", QIcon(":/images/icons/homeFolder.png")),
                         tr("Ctrl+Shift+H"));
    createGotoDirCommand(QDesktopServices::DocumentsLocation, QIcon(":/images/icons/documentsFolder.png"), tr("Ctrl+Shift+O"));
#ifdef Q_OS_MAC
    createGotoDirCommand(QDesktopServices::ApplicationsLocation, QIcon(":/images/icons/appsFolder.png"), tr("Ctrl+Shift+A"));
#endif
    createGotoDirCommand(QDesktopServices::MusicLocation, QIcon(":/images/icons/musicFolder.png"));
    createGotoDirCommand(QDesktopServices::MoviesLocation, QIcon(":/images/icons/movieFolder.png"));
    createGotoDirCommand(QDesktopServices::PicturesLocation, QIcon(":/images/icons/picturesFolder.png"));

    return true;
}

void FileManagerPluginImpl::createGotoDirCommand(QDesktopServices::StandardLocation location, const QIcon &icon, const QKeySequence &key)
{
    GuiSystem::CommandContainer * container = ActionManager::instance()->container(Constants::Ids::Menus::GoTo);
    QDir dir(QDesktopServices::storageLocation(location));

    if (!dir.exists())
        return;

    QString id = QString(QLatin1String(Constants::Ids::Actions::Goto)).arg(location);
    Command *cmd = new Command(id.toLatin1(), this);
    cmd->setData(dir.absolutePath());
    QString displayName(QDesktopServices::displayName(location));
    // fir for broken linux Qt
    if (displayName.isEmpty())
    {
        QString path = dir.absolutePath();
        int index = path.lastIndexOf(QLatin1Char('/'));
        if (index != -1)
            displayName = path.mid(index + 1);
    }

    cmd->setDefaultText(displayName);
    cmd->setDefaultShortcut(key);

    if (!icon.isNull())
        cmd->setDefaultIcon(icon);

    cmd->setData(dir.absolutePath());
    cmd->setContext(Command::ApplicationCommand);

    container->addCommand(cmd, Constants::Ids::MenuGroups::Locations);
}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)
