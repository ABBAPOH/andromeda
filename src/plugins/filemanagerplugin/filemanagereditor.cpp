#include "filemanagereditor.h"

#include "filesystemmodel.h"
#include "fileinfodialog.h"
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QFileIconProvider>

#include <actionmanager.h>
#include <constants.h>
#include <mainwindow.h>
#include <minisplitter.h>
#include <pluginmanager.h>
#include <QHBoxLayout>

using namespace CorePlugin;
using namespace FileManagerPlugin;

FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    bool enableDualPane = settings.value("dualPaneModeEnabled").toBool();
    int mode = settings.value("viewMode").toInt();
    mode = mode == 0 ? 1 : mode;
    mode = enableDualPane ? -1 : mode;
    settings.endGroup();

    FileSystemModel *model = ExtensionSystem::PluginManager::instance()->object<FileSystemModel>("FileSystemModel");

    splitter = new MiniSplitter(this);
    m_widget = new DualPaneWidget(model,  splitter);
    if (!enableDualPane)
        setViewMode(mode);
    m_widget->setDualPaneModeEnabled(enableDualPane);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(currentPathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SLOT(onOpenRequested(QString)));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    m_panel = new NavigationPanel(splitter);
    m_panel->setModel(ExtensionSystem::PluginManager::instance()->object<NavigationModel>("navigationModel"));
    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));

    splitter->addWidget(m_panel);
    splitter->addWidget(m_widget);
    splitter->setSizes(QList<int>() << 100 << 400);

    createActions();
}

bool FileManagerEditor::open(const QString &path)
{
    m_widget->setCurrentPath(path);
    return true;
}

QString FileManagerEditor::currentPath() const
{
    return m_widget->currentPath();
}

int FileManagerEditor::currentIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

void FileManagerEditor::setCurrentIndex(int index)
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

QIcon FileManagerEditor::icon() const
{
    return QFileIconProvider().icon(QFileInfo(currentPath()));
}

QString FileManagerPlugin::FileManagerEditor::title() const
{
    QString path = currentPath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    QFileInfo fi(path);
    if (fi.exists())
        return fi.fileName();
    return QString();
}

QString FileManagerEditor::windowTitle() const
{
    return title();
}

void FileManagerEditor::restoreSession(QSettings &s)
{
    AbstractEditor::restoreSession(s);

    int viewMode = s.value(QLatin1String("viewMode")).toInt();
    bool dualPaneModeEnabled = s.value(QLatin1String("dualPaneModeEnabled")).toBool();
    m_widget->setViewMode((FileManagerWidget::ViewMode)viewMode);
    m_widget->setDualPaneModeEnabled(dualPaneModeEnabled);

    iconModeAction->setChecked(viewMode == 1);
    columnModeAction->setChecked(viewMode == 3);
    treeModeAction->setChecked(viewMode == 4);
    dualPaneModeAction->setChecked(dualPaneModeEnabled);
}

void FileManagerEditor::saveSession(QSettings &s)
{
    AbstractEditor::saveSession(s);
    s.setValue(QLatin1String("viewMode"), (int)m_widget->viewMode());
    s.setValue(QLatin1String("dualPaneModeEnabled"), m_widget->dualPaneModeEnabled());
}

void FileManagerEditor::resizeEvent(QResizeEvent *e)
{
    splitter->resize(e->size());
}

void FileManagerEditor::onOpenRequested(const QString &path)
{
    mainWindow()->open(path);
}

void FileManagerEditor::onCustomContextMenuRequested(const QPoint &pos)
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

void FileManagerEditor::setDualPaneModeEnabled(bool on)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("dualPaneModeEnabled", on);
    settings.endGroup();
    m_widget->setDualPaneModeEnabled(on);
}

void FileManagerEditor::setViewMode(int mode)
{
    setDualPaneModeEnabled(false);

    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("viewMode", mode);
    settings.endGroup();
    m_widget->setViewMode((FileManagerWidget::ViewMode)mode);
}

void FileManagerEditor::showFileInfo()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    if (paths.isEmpty())
        paths.append(m_widget->currentPath());

    foreach (const QString &path, paths) {
        FileInfoDialog *dialog = new FileInfoDialog(m_widget);
        dialog->setFileInfo(QFileInfo(path));
        dialog->show();
    }
}

void FileManagerEditor::createActions()
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    openAction = actionManager->command(Constants::Actions::Open)->action(m_widget, SLOT(open()));
    newFolderAction = actionManager->command(Constants::Actions::NewFolder)->action(m_widget, SLOT(newFolder()));
    renameAction = actionManager->command(Constants::Actions::Rename)->action(m_widget, SLOT(rename()));
    removeAction = actionManager->command(Constants::Actions::Remove)->action(m_widget, SLOT(remove()));

    showFileInfoAction = actionManager->command(Constants::Actions::FileInfo)->action(this);
    connect(showFileInfoAction, SIGNAL(triggered()), SLOT(showFileInfo()));
    m_widget->addAction(showFileInfoAction);

    redoAction = actionManager->command(Constants::Actions::Redo)->action(m_widget, SLOT(redo()));
    undoAction = actionManager->command(Constants::Actions::Undo)->action(m_widget, SLOT(undo()));

    //    actionManager->command(Constants::Actions::Cut)->action(m_widget, SLOT(cut()));
    copyAction = actionManager->command(Constants::Actions::Copy)->action(m_widget, SLOT(copy()));
    pasteAction = actionManager->command(Constants::Actions::Paste)->action(m_widget, SLOT(paste()));
    selectAllAction = actionManager->command(Constants::Actions::SelectAll)->action(m_widget, SLOT(selectAll()));

    actionManager->command(Constants::Actions::Up)->action(m_widget, SLOT(up()));

    showHiddenFilesAction = actionManager->command(Constants::Actions::ShowHiddenFiles)->action(this);
    showHiddenFilesAction->setCheckable(true);
    connect(showHiddenFilesAction, SIGNAL(toggled(bool)), m_widget, SLOT(showHiddenFiles(bool)));
    m_widget->addAction(showHiddenFilesAction);

    int viewMode = m_widget->viewMode();

    QSignalMapper *viewMapper = new QSignalMapper(this);

    iconModeAction = actionManager->command(Constants::Actions::IconMode)->action(viewMapper, SLOT(map()));
    iconModeAction->setCheckable(true);
    viewMapper->setMapping(iconModeAction, 1);
    m_widget->addAction(iconModeAction);

    columnModeAction = actionManager->command(Constants::Actions::ColumnMode)->action(viewMapper, SLOT(map()));
    columnModeAction->setCheckable(true);
    viewMapper->setMapping(columnModeAction, 3);
    m_widget->addAction(columnModeAction);

    treeModeAction = actionManager->command(Constants::Actions::TreeMode)->action(viewMapper, SLOT(map()));
    treeModeAction->setCheckable(true);
    viewMapper->setMapping(treeModeAction, 4);
    m_widget->addAction(treeModeAction);

    dualPaneModeAction = actionManager->command(Constants::Actions::DualPane)->action(this, SLOT(setDualPaneModeEnabled(bool)));
    dualPaneModeAction->setCheckable(true);
    m_widget->addAction(dualPaneModeAction);

    viewModeGroup = new QActionGroup(this);
    viewModeGroup->addAction(iconModeAction);
    viewModeGroup->addAction(columnModeAction);
    viewModeGroup->addAction(treeModeAction);
    viewModeGroup->addAction(dualPaneModeAction);

    iconModeAction->setChecked(viewMode == 1);
    columnModeAction->setChecked(viewMode == 3);
    treeModeAction->setChecked(viewMode == 4);
    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());

    connect(viewMapper, SIGNAL(mapped(int)), SLOT(setViewMode(int)));
}

FileManagerEditorFactory::FileManagerEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QStringList FileManagerEditorFactory::mimeTypes()
{
    return QStringList() << QLatin1String("inode/directory");
}

QByteArray FileManagerEditorFactory::id() const
{
    return "FileManager";
}

AbstractEditor * FileManagerEditorFactory::createEditor(QWidget *parent)
{
    return new FileManagerEditor(parent);
}

