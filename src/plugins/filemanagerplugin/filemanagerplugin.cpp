#include "filemanagerplugin.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDir>
#include <QtGui/QFileIconProvider>
#ifdef Q_CC_MSVC
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#endif

#include <actionmanager.h>
#include <command.h>
#include <commandcontainer.h>
#include <constants.h>
#include <CorePlugin>

#include "filecopydialog.h"
#include "filemanagereditor.h"
#include "filemanagersettingspage.h"
#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "navigationmodel.h"

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace CorePlugin;
using namespace FileManagerPlugin;

FileManagerPluginImpl::FileManagerPluginImpl(QObject *parent) :
    ExtensionSystem::IPlugin()
{
}

bool FileManagerPluginImpl::initialize()
{
    FileManagerEditorFactory *f = new FileManagerEditorFactory(this);
    Core::instance()->editorManager()->addFactory(f);

    NavigationModel *navigationModel = new NavigationModel;
    navigationModel->setObjectName("navigationModel");
    connect(navigationModel, SIGNAL(standardLocationsChanged(NavigationModel::StandardLocations)),
            SLOT(onStandardLocationsChanged(NavigationModel::StandardLocations)));
    addObject(navigationModel);
    CorePlugin::Core::instance()->settings()->addObject(navigationModel, "fileManager/standardLocations");

    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");
    pageManager->addPage(new FileManagerSettingsPage);

    FileSystemModel *model = new FileSystemModel;
    addObject(model);
    addObject(new FileCopyDialog(model->fileSystemManager()), "fileCopyDialog");

    CommandContainer *viewContainer = ActionManager::instance()->container(Constants::Menus::View);

    Command * showHiddenFilesCommand = new Command(Constants::Actions::ShowHiddenFiles, this);
    showHiddenFilesCommand->setDefaultText(tr("Show Hidden Files"));
    showHiddenFilesCommand->setCheckable(true);
    showHiddenFilesCommand->setDefaultShortcut(tr("Ctrl+."));
    showHiddenFilesCommand->setContext(Command::WindowCommand);
    viewContainer->addCommand(showHiddenFilesCommand);

    CommandContainer *goToContainer = ActionManager::instance()->container(Constants::Menus::GoTo);
    // ================ GoTo Menu (Locations) ================
    goToContainer->addGroup(Constants::MenuGroups::Locations);

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

void FileManagerPluginImpl::shutdown()
{
#ifdef Q_CC_MSVC
    qApp->clipboard()->clear();
#endif
}

void FileManagerPluginImpl::createGotoDirCommand(QDesktopServices::StandardLocation location, const QIcon &icon, const QKeySequence &key)
{
    GuiSystem::CommandContainer * container = ActionManager::instance()->container(Constants::Menus::GoTo);
    QDir dir(QDesktopServices::storageLocation(location));

    if (!dir.exists())
        return;

    QString id = QString(QLatin1String(Constants::Actions::Goto)).arg(location);
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

    container->addCommand(cmd, Constants::MenuGroups::Locations);
}

void FileManagerPluginImpl::onStandardLocationsChanged(NavigationModel::StandardLocations loc)
{
    CorePlugin::Core::instance()->settings()->setValue("fileManager/standardLocations", (int)loc);
}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)

