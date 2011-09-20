#include "filemanagerview.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#include <QtGui/QMenu>

#include <actionmanager.h>
#include <command.h>
#include <constants.h>
#include <mainwindow.h>
#include <pluginmanager.h>

#include "dualpanewidget.h"
#include "fileinfodialog.h"
#include "filemanagerwidget.h"
#include "filesystemmodel.h"

using namespace FileManagerPlugin;
using namespace GuiSystem;

FileManagerView::FileManagerView(QObject *parent) :
    IEditor(parent)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    bool enableDualPane = settings.value("dualPaneModeEnabled").toBool();

    FileSystemModel *model = ExtensionSystem::PluginManager::instance()->object<FileSystemModel>("FileSystemModel");

    m_widget = new DualPaneWidget(model);
    m_widget->setDualPaneModeEnabled(enableDualPane);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(pathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SLOT(onOpenRequested(QString)));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    int mode = settings.value("viewMode").toInt();
    mode = mode == 0 ? 1 : mode;
    mode = enableDualPane ? -1 : mode;
    if (!enableDualPane)
        setViewMode(mode);
    settings.endGroup();

    createActions();
}

FileManagerView::~FileManagerView()
{
    delete m_widget;
}

void FileManagerView::initialize()
{
}

QWidget * FileManagerView::widget() const
{
    return m_widget;
}

bool FileManagerView::open(const QString &path)
{
    m_widget->setCurrentPath(path);
    return true;
}

QString FileManagerView::currentPath() const
{
    return m_widget->currentPath();
}

int FileManagerView::currentIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

void FileManagerView::setCurrentIndex(int index)
{
    DualPaneWidget::Pane pane;
    if (index < -1) {
        pane = DualPaneWidget::RightPane;
        index = -index - 2;
    } else {
        pane = DualPaneWidget::LeftPane;
    }
    m_widget->setActivePane(pane);
    m_widget->activeWidget()->history()->setCurrentItemIndex(index);
}

QIcon FileManagerView::icon() const
{
    return QFileIconProvider().icon(QFileInfo(currentPath()));
}

QString FileManagerPlugin::FileManagerView::title() const
{
    QString path = currentPath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    QFileInfo fi(path);
    if (fi.exists())
        return fi.fileName();
    return QString();
}

QString FileManagerView::windowTitle() const
{
    return title();
}

void FileManagerView::restoreSession(const QSettings &s)
{
    m_widget->setViewMode((FileManagerWidget::ViewMode)s.value(QLatin1String("viewMode")).toInt());
    m_widget->setDualPaneModeEnabled(s.value(QLatin1String("dualPaneModeEnabled")).toBool());
    IEditor::restoreSession(s);
}

void FileManagerView::saveSession(QSettings &s)
{
    IEditor::saveSession(s);
    s.setValue(QLatin1String("viewMode"), (int)m_widget->viewMode());
    s.setValue(QLatin1String("dualPaneModeEnabled"), m_widget->dualPaneModeEnabled());
}

void FileManagerView::onOpenRequested(const QString &path)
{
    mainWindow()->open(path);
}

void FileManagerView::onCustomContextMenuRequested(const QPoint &pos)
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();

    QMenu *menu = new QMenu;
    if (paths.isEmpty()) {
        menu->addAction(newFolderAction);
        menu->addSeparator();
        menu->addAction(showFileInfoAction);
        menu->addSeparator();
        menu->addAction(pasteAction);
        menu->addAction(selectAllAction);
        menu->addSeparator();
        QMenu * viewModeMenu = menu->addMenu(tr("View Mode"));
        viewModeMenu->addAction(iconModeAction);
        viewModeMenu->addAction(columnModeAction);
        viewModeMenu->addAction(treeModeAction);
        viewModeMenu->addAction(dualPaneModeAction);
    } else {
        menu->addAction(openAction);
        menu->addSeparator();
        menu->addAction(showFileInfoAction);
        menu->addSeparator();
        menu->addAction(renameAction);
        menu->addAction(removeAction);
        menu->addSeparator();
        menu->addAction(copyAction);
    }
    menu->exec(m_widget->mapToGlobal(pos));
    delete menu;
}

void FileManagerView::setDualPaneModeEnabled(bool on)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("dualPaneModeEnabled", on);
    settings.endGroup();
    m_widget->setDualPaneModeEnabled(on);
}

void FileManagerPlugin::FileManagerView::setViewMode(int mode)
{
    setDualPaneModeEnabled(false);

    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("viewMode", mode);
    settings.endGroup();
    m_widget->setViewMode((FileManagerWidget::ViewMode)mode);
}

QString FileManagerFactory::id() const
{
    return QLatin1String("FileManager");
}

QString FileManagerFactory::type() const
{
    return QLatin1String("FileManager");
}

IView * FileManagerFactory::createView()
{
    return new FileManagerView(this);
}

#include <QApplication>
void FileManagerView::showFileInfo()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    if (paths.isEmpty())
        paths.append(m_widget->currentPath());

    foreach (const QString &path, paths) {
        FileInfoDialog *dialog = new FileInfoDialog(m_widget);
        dialog->setFileInfo(QFileInfo(path));
        dialog->show();
        qApp->processEvents();
    }
}

void FileManagerView::createActions()
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    openAction = actionManager->command(Constants::Ids::Actions::Open)->action(m_widget, SLOT(open()));
    openAction->setShortcut(QKeySequence());
    newFolderAction = actionManager->command(Constants::Ids::Actions::NewFolder)->action(m_widget, SLOT(newFolder()));
    renameAction = actionManager->command(Constants::Ids::Actions::Rename)->action(m_widget, SLOT(rename()));
    renameAction->setShortcut(QKeySequence());
    removeAction = actionManager->command(Constants::Ids::Actions::Remove)->action(m_widget, SLOT(remove()));

    showFileInfoAction = actionManager->command(Constants::Ids::Actions::FileInfo)->action(this);
    connect(showFileInfoAction, SIGNAL(triggered()), SLOT(showFileInfo()));
    m_widget->addAction(showFileInfoAction);

    redoAction = actionManager->command(Constants::Ids::Actions::Redo)->action(m_widget, SLOT(redo()));
    undoAction = actionManager->command(Constants::Ids::Actions::Undo)->action(m_widget, SLOT(undo()));

    //    actionManager->command(Constants::Ids::Actions::Cut)->action(m_widget, SLOT(cut()));
    copyAction = actionManager->command(Constants::Ids::Actions::Copy)->action(m_widget, SLOT(copy()));
    pasteAction = actionManager->command(Constants::Ids::Actions::Paste)->action(m_widget, SLOT(paste()));
    selectAllAction = actionManager->command(Constants::Ids::Actions::SelectAll)->action(m_widget, SLOT(selectAll()));

    actionManager->command(Constants::Ids::Actions::Up)->action(m_widget, SLOT(up()));

    showHiddenFilesAction = actionManager->command(Constants::Ids::Actions::ShowHiddenFiles)->action(this);
    connect(showHiddenFilesAction, SIGNAL(toggled(bool)), m_widget, SLOT(showHiddenFiles(bool)));
    m_widget->addAction(showHiddenFilesAction);

    int viewMode = m_widget->viewMode();

    QSignalMapper *viewMapper = new QSignalMapper(this);

    iconModeAction = actionManager->command(Constants::Ids::Actions::IconMode)->action(this);
    viewMapper->setMapping(iconModeAction, 1);
    connect(iconModeAction, SIGNAL(toggled(bool)), viewMapper, SLOT(map()));
    m_widget->addAction(iconModeAction);
    iconModeAction->setChecked(viewMode == 1);

    columnModeAction = actionManager->command(Constants::Ids::Actions::ColumnMode)->action(this);
    viewMapper->setMapping(columnModeAction, 3);
    connect(columnModeAction, SIGNAL(toggled(bool)), viewMapper, SLOT(map()));
    m_widget->addAction(columnModeAction);
    columnModeAction->setChecked(viewMode == 3);

    treeModeAction = actionManager->command(Constants::Ids::Actions::TreeMode)->action(this);
    viewMapper->setMapping(treeModeAction, 4);
    connect(treeModeAction, SIGNAL(toggled(bool)), viewMapper, SLOT(map()));
    m_widget->addAction(treeModeAction);
    treeModeAction->setChecked(viewMode == 4);

    dualPaneModeAction = actionManager->command(Constants::Ids::Actions::DualPane)->action(this);
    connect(dualPaneModeAction, SIGNAL(toggled(bool)), this, SLOT(setDualPaneModeEnabled(bool)));
    m_widget->addAction(dualPaneModeAction);
    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());

    viewModeGroup = new QActionGroup(this);
    viewModeGroup->addAction(iconModeAction);
    viewModeGroup->addAction(columnModeAction);
    viewModeGroup->addAction(treeModeAction);
    viewModeGroup->addAction(dualPaneModeAction);

    connect(viewMapper, SIGNAL(mapped(int)), SLOT(setViewMode(int)));
}
