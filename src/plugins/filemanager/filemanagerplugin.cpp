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

#include <guisystem/action.h>
#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/commandcontainer.h>
#include <guisystem/mainwindow.h>

#include <guisystem/editormanager.h>
#include <guisystem/settingspagemanager.h>

#include <core/constants.h>

#include "filemanagerconstants.h"
#include "filecopydialog.h"
#include "filemanagereditor.h"
#include "filemanagersettings.h"
#include "filemanagersettingspage.h"
#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "navigationmodel.h"
#include "navigationpanelsettings.h"

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace FileManager;

FileManagerPlugin::FileManagerPlugin(QObject *parent) :
    ExtensionSystem::IPlugin()
{
}

bool FileManagerPlugin::initialize(const QVariantMap &options)
{
    FileManagerEditorFactory *f = new FileManagerEditorFactory(this);
//    Core::instance()->editorManager()->addFactory(f);
    EditorManager::instance()->addFactory(f);

    NavigationModel *navigationModel = new NavigationModel;
    navigationModel->setObjectName("navigationModel");
    addObject(navigationModel);

    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");
    pageManager->addPage(new FileManagerSettingsPage);

    FileSystemModel *model = new FileSystemModel;
    addObject(model);

    addObject(new FileCopyDialog(), "fileCopyDialog");

    createActions();

    loadSettings();

    return true;
}

void FileManagerPlugin::shutdown()
{
#ifdef Q_CC_MSVC
    qApp->clipboard()->clear();
#endif
    saveSettings();
}

void FileManagerPlugin::goTo(const QString &s)
{
    MainWindow *window = MainWindow::currentWindow();
    if (window) {
        window->open(QUrl::fromLocalFile(s));
    }
}

void FileManagerPlugin::createActions()
{
    createFileMenu();
    createViewMenu();
    createGoToMenu();
    createGoToActions();
}

void FileManagerPlugin::createFileMenu()
{
    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    container = ActionManager::instance()->container(Constants::Menus::File);

    cmd = new Command(Constants::Actions::Open, tr("Open"), this);
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence::Open);
#else
//    openCommand->setDefaultShortcut(tr("Return"));
#endif
    cmd->setContext(Command::WidgetCommand);
    container->addCommand(cmd, Constants::MenuGroups::FileNew);

    // ================ File Menu (Info) ================
    group = Constants::MenuGroups::FileInfo;

    cmd = new Command(Constants::Actions::FileInfo, this);
    cmd->setDefaultText(tr("File info"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+I")));
    container->addCommand(cmd, group);

    // ================ File Menu (Change) ================
    group = Constants::MenuGroups::FileChange;

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

void FileManagerPlugin::createViewMenu()
{
    Command *cmd = 0;
    CommandContainer *container = 0;
    const char *group = 0;

    container = ActionManager::instance()->container(Constants::Menus::View);

    // ================ View Menu (View Mode) ================
    group = Constants::MenuGroups::ViewViewMode;

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
    group = Constants::MenuGroups::ViewSortBy;

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

    group = Constants::MenuGroups::ViewSortByOrder;

    cmd = new Command(Constants::Actions::SortByDescendingOrder, this);
    cmd->setDefaultText(tr("Descending Order"));
//    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+4")));
    container->addCommand(cmd, group);
}

void FileManagerPlugin::createGoToMenu()
{
    // ================ GoTo Menu (Locations) ================

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

void FileManagerPlugin::createGoToDirCommand(QDesktopServices::StandardLocation location, const QIcon &icon, const QKeySequence &key)
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

void FileManagerPlugin::createGoToActions()
{
    ActionManager * actionManager = ActionManager::instance();
    CommandContainer *gotoMenu = actionManager->container(Constants::Menus::GoTo);
    gotoMapper = new QSignalMapper(this);
    foreach (Command *cmd, gotoMenu->commands(Constants::MenuGroups::Locations)) {
        QAction *action = new Action(cmd->id(), this);
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
        action->setParent(this);
        actionManager->registerAction(action, cmd->id());
    }
    connect(gotoMapper, SIGNAL(mapped(QString)), this, SLOT(goTo(QString)));
}

void FileManagerPlugin::createSortByActons()
{
}

void FileManagerPlugin::loadSettings()
{
    m_settings = new QSettings(this);
    m_settings->beginGroup(QLatin1String("fileManager"));
    m_fileManagerSettings = FileManagerSettings::globalSettings();
    m_panelSettings = NavigationPanelSettings::globalSettings();

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

    NavigationModel::StandardLocations locations = m_panelSettings->standardLocations();

    locations = NavigationModel::StandardLocations(m_settings->value(QLatin1String("standardLocations"),
                                                                     (int)locations).toInt());

    m_panelSettings->setStandardLocations(locations);
}

void FileManagerPlugin::saveSettings()
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

    NavigationModel::StandardLocations locations = m_panelSettings->standardLocations();

    m_settings->setValue(QLatin1String("standardLocations"), (int)locations);
}

Q_EXPORT_PLUGIN(FileManagerPlugin)
