#include "filemanagerplugin.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtCore/QUrl>
#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#ifdef Q_CC_MSVC
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#endif

#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/commandcontainer.h>

#include <coreplugin/constants.h>
#include <coreplugin/core.h>
#include <guisystem/editormanager.h>
#include <coreplugin/mainwindow.h>
#include <coreplugin/settings.h>
#include <coreplugin/settingspagemanager.h>

#include "filecopydialog.h"
#include "filemanagereditor.h"
#include "filemanagersettings.h"
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

bool FileManagerPluginImpl::initialize(const QVariantMap &options)
{
    FileManagerEditorFactory *f = new FileManagerEditorFactory(this);
//    Core::instance()->editorManager()->addFactory(f);
    EditorManager::instance()->addFactory(f);

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

    addObject(new FileCopyDialog(), "fileCopyDialog");

    createActions();

    loadSettings();

    return true;
}

void FileManagerPluginImpl::shutdown()
{
#ifdef Q_CC_MSVC
    qApp->clipboard()->clear();
#endif
    saveSettings();
}

void FileManagerPluginImpl::onStandardLocationsChanged(NavigationModel::StandardLocations loc)
{
    CorePlugin::Core::instance()->settings()->setValue("fileManager/standardLocations", (int)loc);
}

void FileManagerPluginImpl::goTo(const QString &s)
{
    MainWindow *window = MainWindow::currentWindow();
    if (window) {
        window->open(QUrl::fromLocalFile(s));
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
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+I")));
    container->addCommand(cmd, group);

    // ================ File Menu (Change) ================
    container->addGroup(group = Constants::MenuGroups::FileChange);

    cmd = new Command(Constants::Actions::NewFolder, this);
    cmd->setDefaultText(tr("New folder"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Shift+N")));
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CopyFiles, this);
    cmd->setDefaultText(tr("Copy files"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("F5")));
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::MoveFiles, this);
    cmd->setDefaultText(tr("Move files"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("F6")));
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Rename, this);
    cmd->setDefaultText(tr("Rename"));
#ifdef Q_OS_MAC
//    cmd->setDefaultShortcut(tr("Return")); // Can't set shorcut to prevent overriding edit triggers
#else
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("F2")));
#endif
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::Remove, this);
    cmd->setDefaultText(tr("Remove"));
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Shift+Backspace")));
#else
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Shift+Del")));
#endif
    container->addCommand(cmd, group);
}

void FileManagerPluginImpl::createViewMenu()
{
    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    container = ActionManager::instance()->container(Constants::Menus::View);

    // ================ View Menu (View Mode) ================
    container->addGroup(group = Constants::MenuGroups::ViewViewMode);

    cmd = new Command(Constants::Actions::IconMode, this);
    cmd->setDefaultText(tr("Icon View"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+1")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::ColumnMode, this);
    cmd->setDefaultText(tr("Column View"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+2")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::TreeMode, this);
    cmd->setDefaultText(tr("Tree View"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+3")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::CoverFlowMode, this);
    cmd->setDefaultText(tr("Cover flow"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+4")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::DualPane, this);
    cmd->setDefaultText(tr("Dual Pane"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+5")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd, group);

    cmd = new Command(Constants::Actions::ShowHiddenFiles, this);
    cmd->setDefaultText(tr("Show Hidden Files"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+.")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::ShowLeftPanel, this);
    cmd->setDefaultText(tr("Show left panel"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+L")));
    cmd->setContext(Command::WindowCommand);
    container->addCommand(cmd);

    // ================ View Menu (View Mode) ================
    container->addGroup(group = Constants::MenuGroups::ViewSortBy);
    CommandContainer *sortByMenu = new CommandContainer(Constants::Menus::SortBy, this);
    sortByMenu->setTitle(tr("Sort by"));
    container->addContainer(sortByMenu, group);
    container = sortByMenu;

    cmd = new Command(Constants::Actions::SortByName, this);
    cmd->setDefaultText(tr("Sort by name"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+1")));
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::SortBySize, this);
    cmd->setDefaultText(tr("Sort by size"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+2")));
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::SortByType, this);
    cmd->setDefaultText(tr("Sort by type"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+3")));
    container->addCommand(cmd);

    cmd = new Command(Constants::Actions::SortByDate, this);
    cmd->setDefaultText(tr("Sort by date"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+4")));
    container->addCommand(cmd);

    container->addGroup(group = Constants::MenuGroups::ViewSortByOrder);

    cmd = new Command(Constants::Actions::SortByDescendingOrder, this);
    cmd->setDefaultText(tr("Descending Order"));
//    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+4")));
    container->addCommand(cmd, group);
}

void FileManagerPluginImpl::createGoToMenu()
{
    CommandContainer *container = ActionManager::instance()->container(Constants::Menus::GoTo);

    // ================ GoTo Menu (Locations) ================
    container->addGroup(Constants::MenuGroups::Locations);

    createGoToDirCommand(QDesktopServices::DesktopLocation,
                         QIcon(":/images/icons/desktopFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+D")));
    createGoToDirCommand(QDesktopServices::HomeLocation,
                         QIcon::fromTheme("go-home", QIcon(":/images/icons/homeFolder.png")),
                         QKeySequence(QLatin1String("Ctrl+Shift+H")));
    createGoToDirCommand(QDesktopServices::DocumentsLocation,
                         QIcon(":/images/icons/documentsFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+O")));
#ifdef Q_OS_MAC
    createGoToDirCommand(QDesktopServices::ApplicationsLocation,
                         QIcon(":/images/icons/appsFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+A")));
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

void FileManagerPluginImpl::createSortByActons()
{
}

void FileManagerPluginImpl::loadSettings()
{
    m_settings = new QSettings(this);
    m_settings->beginGroup(QLatin1String("fileManager"));
    m_fileManagerSettings = FileManagerSettings::globalSettings();

    QSize iconSize = m_fileManagerSettings->iconSize(FileManagerSettings::IconView);
    QSize columnIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::ColumnView);
    QSize treeIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::TreeView);
    QSize gridSize = m_fileManagerSettings->gridSize();
    int flow = m_fileManagerSettings->flow();
    bool itemsExpandable = m_fileManagerSettings->itemsExpandable();

    iconSize = m_settings->value(QLatin1String("iconMode"), iconSize).toSize();
    columnIconSize = m_settings->value(QLatin1String("columnIconSize"), columnIconSize).toSize();
    treeIconSize = m_settings->value(QLatin1String("treeIconSize"), treeIconSize).toSize();
    gridSize = m_settings->value(QLatin1String("gridSize"), gridSize).toSize();
    flow = m_settings->value(QLatin1String("flow"), flow).toInt();
    itemsExpandable = m_settings->value(QLatin1String("itemsExpandable"), itemsExpandable).toBool();

    m_fileManagerSettings->setIconSize(FileManagerSettings::IconView, iconSize);
    m_fileManagerSettings->setIconSize(FileManagerSettings::ColumnView, columnIconSize);
    m_fileManagerSettings->setIconSize(FileManagerSettings::TreeView, treeIconSize);
    m_fileManagerSettings->setGridSize(gridSize);
    m_fileManagerSettings->setFlow((FileManagerSettings::Flow)flow);
    m_fileManagerSettings->setItemsExpandable(itemsExpandable);
}

void FileManagerPluginImpl::saveSettings()
{
    QSize iconSize = m_fileManagerSettings->iconSize(FileManagerSettings::IconView);
    QSize columnIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::ColumnView);
    QSize treeIconSize = m_fileManagerSettings->iconSize(FileManagerSettings::TreeView);
    QSize gridSize = m_fileManagerSettings->gridSize();
    FileManagerSettings::Flow flow = m_fileManagerSettings->flow();
    bool itemsExpandable = m_fileManagerSettings->itemsExpandable();

    m_settings->setValue(QLatin1String("iconMode"), iconSize);
    m_settings->setValue(QLatin1String("columnIconSize"), columnIconSize);
    m_settings->setValue(QLatin1String("treeIconSize"), treeIconSize);
    m_settings->setValue(QLatin1String("gridSize"), gridSize);
    m_settings->setValue(QLatin1String("flow"), flow);
    m_settings->setValue(QLatin1String("itemsExpandable"), itemsExpandable);
}

Q_EXPORT_PLUGIN(FileManagerPluginImpl)
