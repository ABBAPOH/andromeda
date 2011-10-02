#include "filemanagerplugin.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDir>
#include <QtCore/QSignalMapper>
#include <QtGui/QAction>
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

    createActions();

    return true;
}

void FileManagerPluginImpl::shutdown()
{
#ifdef Q_CC_MSVC
    qApp->clipboard()->clear();
#endif
}

void FileManagerPluginImpl::onStandardLocationsChanged(NavigationModel::StandardLocations loc)
{
    CorePlugin::Core::instance()->settings()->setValue("fileManager/standardLocations", (int)loc);
}

void FileManagerPluginImpl::goTo(const QString &s)
{
    MainWindow *window = MainWindow::currentWindow();
    if (window) {
        window->open(s);
    }
}

void FileManagerPluginImpl::createActions()
{
    createFileMenu();
    createViewMenu();
    createGoToMenu();
    createGoToActions();
}

void FileManagerPluginImpl::createFileMenu()
{
    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    container = ActionManager::instance()->container(Constants::Menus::File);

    // ================ File Menu (Info) ================
    container->addGroup(group = Constants::MenuGroups::FileInfo);

    cmd = new Command(Constants::Actions::FileInfo, this);
    cmd->setDefaultText(tr("File info"));
    cmd->setDefaultShortcut(tr("Ctrl+I"));
    container->addCommand(cmd, group);

    // ================ File Menu (Change) ================
    container->addGroup(group = Constants::MenuGroups::FileChange);

    cmd = new Command(Constants::Actions::NewFolder, this);
    cmd->setDefaultText(tr("New folder"));
    cmd->setDefaultShortcut(tr("Ctrl+Shift+N"));
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Rename, this);
    cmd->setDefaultText(tr("Rename"));
#ifdef Q_OS_MAC
//    renameCommand->setDefaultShortcut(tr("Return")); // Can't set shorcut to prevent overriding edit triggers
#else
    renameCommand->setDefaultShortcut(tr("F2"));
#endif
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Remove, this);
    cmd->setDefaultText(tr("Remove"));
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(tr("Ctrl+Shift+Backspace"));
#else
    removeCommand->setDefaultShortcut(tr("Shift+Del"));
#endif
    container->addCommand(cmd, group);
}

void FileManagerPluginImpl::createViewMenu()
{
    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    container = ActionManager::instance()->container(Constants::Menus::View);

    // ================ GoTo Menu (View Mode) ================
    container->addGroup(group = Constants::MenuGroups::ViewViewMode);

    cmd = new Command(Constants::Actions::IconMode, this);
    cmd->setDefaultText(tr("Icon View"));
    cmd->setDefaultShortcut(tr("Ctrl+1"));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::ColumnMode, this);
    cmd->setDefaultText(tr("Column View"));
    cmd->setDefaultShortcut(tr("Ctrl+2"));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::TreeMode, this);
    cmd->setDefaultText(tr("Tree View"));
    cmd->setDefaultShortcut(tr("Ctrl+3"));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CoverFlowMode, this);
    cmd->setDefaultText(tr("Cover flow"));
    cmd->setDefaultShortcut(tr("Ctrl+4"));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::DualPane, this);
    cmd->setDefaultText(tr("Dual Pane"));
    cmd->setDefaultShortcut(tr("Ctrl+5"));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::ShowHiddenFiles, this);
    cmd->setDefaultText(tr("Show Hidden Files"));
    cmd->setDefaultShortcut(tr("Ctrl+."));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd);
}

void FileManagerPluginImpl::createGoToMenu()
{
    CommandContainer *container = ActionManager::instance()->container(Constants::Menus::GoTo);

    // ================ GoTo Menu (Locations) ================
    container->addGroup(Constants::MenuGroups::Locations);

    createGoToDirCommand(QDesktopServices::DesktopLocation, QIcon(":/images/icons/desktopFolder.png"), tr("Ctrl+Shift+D"));
    createGoToDirCommand(QDesktopServices::HomeLocation,
                         QIcon::fromTheme("go-home", QIcon(":/images/icons/homeFolder.png")),
                         tr("Ctrl+Shift+H"));
    createGoToDirCommand(QDesktopServices::DocumentsLocation, QIcon(":/images/icons/documentsFolder.png"), tr("Ctrl+Shift+O"));
#ifdef Q_OS_MAC
    createGoToDirCommand(QDesktopServices::ApplicationsLocation, QIcon(":/images/icons/appsFolder.png"), tr("Ctrl+Shift+A"));
#endif
    createGoToDirCommand(QDesktopServices::MusicLocation, QIcon(":/images/icons/musicFolder.png"));
    createGoToDirCommand(QDesktopServices::MoviesLocation, QIcon(":/images/icons/movieFolder.png"));
    createGoToDirCommand(QDesktopServices::PicturesLocation, QIcon(":/images/icons/picturesFolder.png"));
}

void FileManagerPluginImpl::createGoToDirCommand(QDesktopServices::StandardLocation location, const QIcon &icon, const QKeySequence &key)
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

void FileManagerPluginImpl::createGoToActions()
{
    ActionManager * actionManager = ActionManager::instance();
    CommandContainer *gotoMenu = actionManager->container(Constants::Menus::GoTo);
    gotoMapper = new QSignalMapper(this);
    foreach (Command *cmd, gotoMenu->commands(Constants::MenuGroups::Locations)) {
        QAction *action = cmd->commandAction();
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
        action->setParent(this);
        actionManager->registerAction(action, cmd->id());
    }
    connect(gotoMapper, SIGNAL(mapped(QString)), this, SLOT(goTo(QString)));

}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)

