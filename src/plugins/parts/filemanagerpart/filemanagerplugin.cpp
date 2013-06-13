#include "filemanagerplugin.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtCore/QUrl>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QFileIconProvider>
#else
#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#endif

#ifdef Q_CC_MSVC
#if QT_VERSION >= 0x050000
#include <QtWidgets/QApplication>
#include <QtWidgets/QClipboard>
#else
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#endif
#endif

#include <Parts/ActionManager>
#include <Parts/Command>
#include <Parts/CommandContainer>
#include <Parts/EditorWindow>

#include <Parts/constants.h>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>
#include <Parts/SettingsPageManager>
#include <Parts/SharedProperties>
#include <Parts/StandardCommands>
#include <Parts/ToolWidgetManager>

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
using namespace Parts;
using namespace FileManager;

FileManagerPlugin *m_instance = 0;

FileManagerPlugin::FileManagerPlugin() :
    ExtensionSystem::IPlugin()
{
    m_instance = this;
}

FileManagerPlugin::~FileManagerPlugin()
{
    m_instance = 0;
}

bool FileManagerPlugin::initialize()
{
    m_properties = new SharedProperties(this);
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

FileManagerPlugin * FileManagerPlugin::instance()
{
    Q_ASSERT_X(m_instance,
               "FileManagerPlugin::instance",
               "Must construct FileManagerPlugin before calling instance()");
    return m_instance;
}

SharedProperties * FileManagerPlugin::properties() const
{
    return m_properties;
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

    createGoToDirCommand(QStandardPaths::DesktopLocation,
                         QIcon(":/images/icons/desktopFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+D")));
    createGoToDirCommand(QStandardPaths::HomeLocation,
                         QIcon::fromTheme("go-home", QIcon(":/images/icons/homeFolder.png")),
                         QKeySequence(QLatin1String("Ctrl+Shift+H")));
    createGoToDirCommand(QStandardPaths::DocumentsLocation,
                         QIcon(":/images/icons/documentsFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+O")));
#ifdef Q_OS_MAC
    createGoToDirCommand(QStandardPaths::ApplicationsLocation,
                         QIcon(":/images/icons/appsFolder.png"),
                         QKeySequence(QLatin1String("Ctrl+Shift+A")));
#endif
    createGoToDirCommand(QStandardPaths::MusicLocation, QIcon(":/images/icons/musicFolder.png"));
    createGoToDirCommand(QStandardPaths::MoviesLocation, QIcon(":/images/icons/movieFolder.png"));
    createGoToDirCommand(QStandardPaths::PicturesLocation, QIcon(":/images/icons/picturesFolder.png"));
}

void FileManagerPlugin::createGoToDirCommand(QStandardPaths::StandardLocation location, const QIcon &icon, const QKeySequence &key)
{
    Parts::CommandContainer * container = ActionManager::instance()->container(Constants::Menus::GoTo);
    QDir dir(QStandardPaths::writableLocation(location));

    if (!dir.exists())
        return;

    QString id = QString(QLatin1String(Constants::Actions::Goto)).arg(location);
    Command *cmd = new Command(id.toLatin1(), this);
    cmd->setAttributes(Command::AttributeUpdateEnabled);
    cmd->setData(dir.absolutePath());
    QString displayName(QStandardPaths::displayName(location));
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
    foreach (AbstractCommand *c, gotoMenu->commands()) {
        Command *cmd = qobject_cast<Command *>(c);
        // TODO: refactor
        if (!cmd->id().startsWith("Goto "))
            continue;
        QAction *action = cmd->commandAction();
        gotoMapper->setMapping(action, cmd->data().toString());
        connect(action, SIGNAL(triggered()), gotoMapper, SLOT(map()));
    }
}

void FileManagerPlugin::loadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("fileManager"));
    m_properties->read(&settings);

    m_fileManagerSettings = FileManagerSettings::globalSettings();
    m_panelSettings = NavigationPanelSettings::globalSettings();

    bool warnOnFileRemove = m_fileManagerSettings->warnOnFileRemove();
    bool warnOnExtensionChange = m_fileManagerSettings->warnOnExtensionChange();

    warnOnFileRemove = settings.value(QLatin1String("warnOnFileRemove"), warnOnFileRemove).toBool();
    warnOnExtensionChange = settings.value(QLatin1String("warnOnExtensionChange"), warnOnExtensionChange).toBool();

    m_fileManagerSettings->setWarnOnFileRemove(warnOnFileRemove);
    m_fileManagerSettings->setWarnOnExtensionChange(warnOnExtensionChange);

    NavigationModel::StandardLocations locations = m_panelSettings->standardLocations();

    locations = NavigationModel::StandardLocations(settings.value(QLatin1String("standardLocations"),
                                                                     (int)locations).toInt());

    m_panelSettings->setStandardLocations(locations);
}

void FileManagerPlugin::saveSettings()
{
    bool warnOnFileRemove = m_fileManagerSettings->warnOnFileRemove();
    bool warnOnExtensionChange = m_fileManagerSettings->warnOnExtensionChange();

    QSettings settings;
    settings.beginGroup(QLatin1String("fileManager"));
    m_properties->write(&settings);

    settings.setValue(QLatin1String("warnOnFileRemove"), warnOnFileRemove);
    settings.setValue(QLatin1String("warnOnExtensionChange"), warnOnExtensionChange);

    NavigationModel::StandardLocations locations = m_panelSettings->standardLocations();

    settings.setValue(QLatin1String("standardLocations"), (int)locations);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(FileManagerPlugin)
#endif
