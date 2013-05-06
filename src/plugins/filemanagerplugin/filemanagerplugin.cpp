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

#include <GuiSystem/Action>
#include <GuiSystem/ActionManager>
#include <GuiSystem/Command>
#include <GuiSystem/CommandContainer>
#include <GuiSystem/EditorWindow>

#include <GuiSystem/constants.h>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>
#include <GuiSystem/SettingsPageManager>
#include <GuiSystem/StandardCommands>
#include <GuiSystem/ToolWidgetManager>

#include <FileManager/constants.h>
#include <FileManager/FileCopyDialog>
#include <FileManager/FileManagerSettings>
#include <FileManager/FileSystemManager>
#include <FileManager/FileSystemModel>
#include <FileManager/NavigationModel>
#include <FileManager/NavigationPanelSettings>

#include "filemanagerdocument.h"
#include "filemanagereditor.h"
#include "viewmodessettings.h"
#include "globalsettings.h"
#include "filesystemtoolwidget.h"

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace FileManager;

FileManagerPlugin::FileManagerPlugin(QObject *parent) :
    ExtensionSystem::IPlugin()
{
}

bool FileManagerPlugin::initialize()
{
    DocumentManager::instance()->addFactory(new FileManagerDocumentFactory(this));
    EditorManager::instance()->addFactory(new FileManagerEditorFactory(this));
    ToolWidgetManager::instance()->addFactory(new FileSystemToolWidgetFactory(this));

    NavigationModel *navigationModel = new NavigationModel;
    navigationModel->setObjectName("navigationModel");
    addObject(navigationModel);
    connect(navigationModel, SIGNAL(pathsDropped(QString,QStringList,Qt::DropAction)),
            SLOT(onPathsDropped(QString,QStringList,Qt::DropAction)));

    SettingsPageManager *pageManager = object<SettingsPageManager>("settingsPageManager");
    pageManager->addPage(new GlobalSettingsPage(this));
    pageManager->addPage(new ViewModesSettingsPage(this));

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
    EditorWindow *window = EditorWindow::currentWindow();
    if (window) {
        window->open(QUrl::fromLocalFile(s));
    }
}

/*!
 \internal
*/
void FileManagerPlugin::onPathsDropped(const QString &destination, const QStringList &paths, Qt::DropAction action)
{
    FileSystemManager *fsManager = FileSystemManager::instance();
    if (action == Qt::CopyAction)
        fsManager->copy(paths, destination);
    else if (action == Qt::MoveAction)
        fsManager->move(paths, destination);
    else if (action == Qt::LinkAction)
        fsManager->link(paths, destination);
}

void FileManagerPlugin::createActions()
{
    Command *cmd = 0;
    cmd = new Command(Constants::Actions::OpenEditor, tr("Open in internal editor"), this);
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence("Ctrl+E"));
#else
    cmd->setDefaultShortcut(QKeySequence("F4"));
#endif

    cmd = new Command(Constants::Actions::ShowFileInfo, this);
    cmd->setText(tr("File info"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+I"));

    cmd = new Command(Constants::Actions::NewFolder, this);
    cmd->setText(tr("New folder"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+Shift+N"));

    cmd = new Command(Constants::Actions::Rename, this);
    cmd->setText(tr("Rename"));
#ifdef Q_OS_MAC
//    cmd->setDefaultShortcut(QKeySequence("Return")); // Can't set shorcut to prevent overriding edit triggers
#else
    cmd->setDefaultShortcut(QKeySequence("F2"));
#endif

    cmd = new Command(Constants::Actions::MoveToTrash, this);
    cmd->setText(tr("Move to trash"));
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence("Ctrl+Backspace"));
#else
    cmd->setDefaultShortcut(QKeySequence("Del"));
#endif

    cmd = new Command(Constants::Actions::Remove, this);
    cmd->setText(tr("Remove"));
#ifdef Q_OS_MAC
    cmd->setDefaultShortcut(QKeySequence("Ctrl+Shift+Backspace"));
#else
    cmd->setDefaultShortcut(QKeySequence("Shift+Del"));
#endif

    cmd = new Command(Constants::Actions::MoveHere, this);
    cmd->setText(tr("Move object(s) here"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+Shift+V"));

    cmd = new Command(Constants::Actions::ShowHiddenFiles, this);
    cmd->setText(tr("Show Hidden Files"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+."));

    QActionGroup * viewGroup = new QActionGroup(this);

    cmd = new Command(Constants::Actions::IconMode, this);
    cmd->setText(tr("Icon View"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+1"));
    viewGroup->addAction(cmd->commandAction());

    cmd = new Command(Constants::Actions::ColumnMode, this);
    cmd->setText(tr("Column View"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+2"));
    viewGroup->addAction(cmd->commandAction());

    cmd = new Command(Constants::Actions::TreeMode, this);
    cmd->setText(tr("Tree View"));
    cmd->setDefaultShortcut(QKeySequence("Ctrl+3"));
    viewGroup->addAction(cmd->commandAction());

    createSortByMenu();
    createPanesMenu();
    createGoToMenu();
    connectGoToActions();
}

void FileManagerPlugin::createSortByMenu()
{
    Command *cmd = 0;
    QActionGroup * sortGroup = new QActionGroup(this);

    CommandContainer *sortByMenu = new CommandContainer(Constants::Menus::SortBy, this);
    sortByMenu->setText(tr("Sort by"));

    cmd = new Command(Constants::Actions::SortByName, this);
    cmd->setText(tr("Sort by name"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+1")));
    sortByMenu->addCommand(cmd);
    sortGroup->addAction(cmd->commandAction());

    cmd = new Command(Constants::Actions::SortBySize, this);
    cmd->setText(tr("Sort by size"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+2")));
    sortByMenu->addCommand(cmd);
    sortGroup->addAction(cmd->commandAction());

    cmd = new Command(Constants::Actions::SortByType, this);
    cmd->setText(tr("Sort by type"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+3")));
    sortByMenu->addCommand(cmd);
    sortGroup->addAction(cmd->commandAction());

    cmd = new Command(Constants::Actions::SortByDate, this);
    cmd->setText(tr("Sort by date"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+4")));
    sortByMenu->addCommand(cmd);
    sortGroup->addAction(cmd->commandAction());

    sortByMenu->addSeparator();

    cmd = new Command(Constants::Actions::SortByDescendingOrder, this);
    cmd->setText(tr("Descending Order"));
//    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+4")));
    sortByMenu->addCommand(cmd);
}

void FileManagerPlugin::createGoToMenu()
{
    CommandContainer *goToMenu = new CommandContainer(Constants::Menus::GoTo, this);
    goToMenu->setText(tr("Go to"));

    goToMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Back));
    goToMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Forward));
    goToMenu->addCommand(StandardCommands::standardCommand(StandardCommands::Up));
    goToMenu->addSeparator();

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
    cmd->setAttributes(Command::AttributeUpdateEnabled);
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

    cmd->setText(displayName);
    cmd->setDefaultShortcut(key);

    if (!icon.isNull())
        cmd->setIcon(icon);

    cmd->setData(dir.absolutePath());

    container->addCommand(cmd);
}

void FileManagerPlugin::connectGoToActions()
{
    ActionManager * actionManager = ActionManager::instance();
    CommandContainer *gotoMenu = actionManager->container(Constants::Menus::GoTo);
    gotoMapper = new QSignalMapper(this);
    connect(gotoMapper, SIGNAL(mapped(QString)), this, SLOT(goTo(QString)));
    foreach (Command *cmd, gotoMenu->commands()) {
        QAction *action = cmd->commandAction();
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
    }
}

void FileManagerPlugin::createPanesMenu()
{
    Command *cmd = 0;

    CommandContainer *panesMenu = new CommandContainer("Panes", this);
    panesMenu->setText(tr("Panes"));

    cmd = new Command(Constants::Actions::DualPane, this);
    cmd->setText(tr("Dual Pane"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+U")));
    panesMenu->addCommand(cmd);

    cmd = new Command(Constants::Actions::VerticalPanels, this);
    cmd->setText(tr("Vertical panes"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Shift+U")));
    panesMenu->addCommand(cmd);

    cmd = new Command(Constants::Actions::ToggleActivePane, this);
    cmd->setText(tr("Toggle active pane"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("Ctrl+Alt+U")));
    panesMenu->addCommand(cmd);

    panesMenu->addCommand(new Separator(this));

    cmd = new Command(Constants::Actions::SyncPanes, tr("Sync panes"), this);
    panesMenu->addCommand(cmd);

    cmd = new Command(Constants::Actions::SwapPanes, tr("Swap panes"), this);
    panesMenu->addCommand(cmd);

    panesMenu->addCommand(new Separator(this));

    cmd = new Command(Constants::Actions::CopyFiles, this);
    cmd->setText(tr("Copy files"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("F5")));
    panesMenu->addCommand(cmd);

    cmd = new Command(Constants::Actions::MoveFiles, this);
    cmd->setText(tr("Move files"));
    cmd->setDefaultShortcut(QKeySequence(QLatin1String("F6")));
    panesMenu->addCommand(cmd);
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
    bool warnOnFileRemove = m_fileManagerSettings->warnOnFileRemove();
    bool warnOnExtensionChange = m_fileManagerSettings->warnOnExtensionChange();

    iconSize = m_settings->value(QLatin1String("iconMode"), iconSize).toSize();
    columnIconSize = m_settings->value(QLatin1String("columnIconSize"), columnIconSize).toSize();
    treeIconSize = m_settings->value(QLatin1String("treeIconSize"), treeIconSize).toSize();
    gridSize = m_settings->value(QLatin1String("gridSize"), gridSize).toSize();
    flow = m_settings->value(QLatin1String("flow"), flow).toInt();
    itemsExpandable = m_settings->value(QLatin1String("itemsExpandable"), itemsExpandable).toBool();
    warnOnFileRemove = m_settings->value(QLatin1String("warnOnFileRemove"), warnOnFileRemove).toBool();
    warnOnExtensionChange = m_settings->value(QLatin1String("warnOnExtensionChange"), warnOnExtensionChange).toBool();

    m_fileManagerSettings->setIconSize(FileManagerSettings::IconView, iconSize);
    m_fileManagerSettings->setIconSize(FileManagerSettings::ColumnView, columnIconSize);
    m_fileManagerSettings->setIconSize(FileManagerSettings::TreeView, treeIconSize);
    m_fileManagerSettings->setGridSize(gridSize);
    m_fileManagerSettings->setFlow((FileManagerSettings::Flow)flow);
    m_fileManagerSettings->setItemsExpandable(itemsExpandable);
    m_fileManagerSettings->setWarnOnFileRemove(warnOnFileRemove);
    m_fileManagerSettings->setWarnOnExtensionChange(warnOnExtensionChange);

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
    bool warnOnFileRemove = m_fileManagerSettings->warnOnFileRemove();
    bool warnOnExtensionChange = m_fileManagerSettings->warnOnExtensionChange();

    m_settings->setValue(QLatin1String("iconMode"), iconSize);
    m_settings->setValue(QLatin1String("columnIconSize"), columnIconSize);
    m_settings->setValue(QLatin1String("treeIconSize"), treeIconSize);
    m_settings->setValue(QLatin1String("gridSize"), gridSize);
    m_settings->setValue(QLatin1String("flow"), flow);
    m_settings->setValue(QLatin1String("itemsExpandable"), itemsExpandable);
    m_settings->setValue(QLatin1String("warnOnFileRemove"), warnOnFileRemove);
    m_settings->setValue(QLatin1String("warnOnExtensionChange"), warnOnExtensionChange);

    NavigationModel::StandardLocations locations = m_panelSettings->standardLocations();

    m_settings->setValue(QLatin1String("standardLocations"), (int)locations);
}

Q_EXPORT_PLUGIN(FileManagerPlugin)
