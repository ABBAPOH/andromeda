#include "filemanagereditor.h"

#include "filesystemmanager.h"
#include "filesystemmodel.h"
#include "fileinfodialog.h"
#include "dualpanewidget.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>
#include <QtCore/QUrl>
#include <QtGui/QResizeEvent>
#include <QtGui/QMenu>
#include <QtGui/QFileIconProvider>

#include <actionmanager.h>
#include <constants.h>
#include <core.h>
#include <mainwindow.h>
#include <minisplitter.h>
#include <pluginmanager.h>
#include <settings.h>

static QVariantList intListToVariantList(const QList<int> list)
{
    QVariantList lst;
    foreach (int i, list) {
        lst.append(i);
    }
    return lst;
}

static QList<int> variantListToIntList(const QVariantList &list)
{
    QList<int> result;
    foreach (const QVariant &v, list) {
        result.append(v.toInt());
    }
    return result;
}

using namespace CorePlugin;
using namespace FileManagerPlugin;

FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(parent)
{
    setupUi();
    setupConnections();
    createActions();
    restoreDefaults();
}

/*!
  \reimp
*/
bool FileManagerEditor::open(const QUrl &url)
{
    m_widget->setCurrentPath(url.toLocalFile());
    return true;
}

/*!
  \reimp
*/
QUrl FileManagerEditor::currentUrl() const
{
    return QUrl::fromLocalFile(m_widget->currentPath());
}

/*!
  \reimp
*/
int FileManagerEditor::currentIndex() const
{
    if (m_widget->activePane() == DualPaneWidget::LeftPane)
        return m_widget->leftWidget()->history()->currentItemIndex(); // 0, 1, 2
    else
        return -m_widget->rightWidget()->history()->currentItemIndex() - 2; // -2, -3, -4
}

/*!
  \reimp
*/
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

/*!
  \reimp
*/
QIcon FileManagerEditor::icon() const
{
    return QFileIconProvider().icon(QFileInfo(m_widget->currentPath()));
}

/*!
  \reimp
*/
QString FileManagerEditor::title() const
{
    QString path = m_widget->currentPath();
    if (path.endsWith(QLatin1Char('/')))
        path = path.left(path.length() - 1);

    QFileInfo fi(path);
    if (fi.exists())
        return fi.fileName();
    return QString();
}

/*!
  \reimp
*/
QString FileManagerEditor::windowTitle() const
{
    return title();
}

/*!
  \reimp
*/
void FileManagerEditor::restoreSession(QSettings &s)
{
    AbstractEditor::restoreSession(s);

    int viewMode = s.value(QLatin1String("viewMode")).toInt();
    setViewMode(viewMode);

    iconModeAction->setChecked(viewMode == IconView);
    columnModeAction->setChecked(viewMode == ColumnView);
    treeModeAction->setChecked(viewMode == TreeView);
    coverFlowModeAction->setChecked(viewMode == CoverFlow);
    dualPaneModeAction->setChecked(viewMode == DualPane);

    QVariantList lst = s.value(QLatin1String("splitterSizes")).toList();
    splitter->setSizes(variantListToIntList(lst));
}

/*!
  \reimp
*/
void FileManagerEditor::saveSession(QSettings &s)
{
    AbstractEditor::saveSession(s);

    int mode = 0;
    if (m_widget->dualPaneModeEnabled())
        mode = DualPane;
    else
        mode = m_widget->viewMode();

    s.setValue(QLatin1String("viewMode"), mode);
    s.setValue(QLatin1String("splitterSizes"), intListToVariantList(splitter->sizes()));
}

/*!
  \reimp
*/
void FileManagerEditor::resizeEvent(QResizeEvent *e)
{
    splitter->resize(e->size());
}

/*!
  \internal

  Reemits DualPaneWidget::currentPathChanged() signal as an QUrl
*/
void FileManagerEditor::onCurrentPathChanged(const QString &path)
{
    emit currentUrlChanged(QUrl::fromLocalFile(path));
}

/*!
  \internal

  Opens file in current tab
*/
void FileManagerEditor::onOpenRequested(const QString &path)
{
    mainWindow()->open(QUrl::fromLocalFile(path));
}

/*!
  \internal

  Shows DualPaneWidget's context menu
*/
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
        viewModeMenu->addAction(coverFlowModeAction);
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

/*!
  \internal

  Sets DualPaneWidget's view mode.
*/
void FileManagerEditor::setViewMode(int mode)
{
    if (mode == DualPane) {
        m_widget->setDualPaneModeEnabled(true);
    } else {
        m_widget->setDualPaneModeEnabled(false);
        m_widget->setViewMode((FileManagerWidget::ViewMode)mode);
    }
}

/*!
  \internal

  Sets DualPaneWidget's view mode and stores is to settings as default mode.
*/
void FileManagerEditor::setAndSaveViewMode(int mode)
{
    m_settings->setValue(QLatin1String("fileManager/viewMode"), mode);

    setViewMode(mode);
}

/*!
  \internal

  Shows or hides left panel and sets default visibility.
*/
void FileManagerEditor::showLeftPanel(bool show)
{
    m_settings->setValue(QLatin1String("fileManager/showLeftPanel"), show);

    m_panel->setVisible(show);
}

/*!
  \internal

  Shows FileInfoDialog.
*/
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

/*!
  \internal

  Updates FileManager actions that depends on selection.
*/
void FileManagerEditor::onSelectedPathsChanged()
{
    QStringList paths = m_widget->activeWidget()->selectedPaths();
    bool enabled = !paths.empty();

    openAction->setEnabled(enabled);
    renameAction->setEnabled(enabled);
    removeAction->setEnabled(enabled);
//    cutAction->setEnabled(enabled);
    copyAction->setEnabled(enabled);

    if (!paths.isEmpty()) {
        if (paths.size() == 1) {
            cutAction->setText(tr("Cut \"%1\"").arg(QFileInfo(paths[0]).fileName()));
            copyAction->setText(tr("Copy \"%1\"").arg(QFileInfo(paths[0]).fileName()));
        } else {
            cutAction->setText(tr("Cut %1 items").arg(paths.size()));
            copyAction->setText(tr("Copy %1 items").arg(paths.size()));
        }
    } else {
        cutAction->setText(tr("Copy"));
        copyAction->setText(tr("Copy"));
    }
}

/*!
 \internal

 Stores default splitter size.
*/
void FileManagerEditor::onSplitterMoved(int, int)
{
    QVariant list = intListToVariantList(splitter->sizes());
    Core::instance()->settings()->setValue("fileManager/lastSplitterSizes", list);
}

/*!
 \internal

*/
void FileManagerEditor::onPathsDropped(const QString &destination, const QStringList &paths, Qt::DropAction action)
{
    FileSystemManager *fsManager = m_widget->leftWidget()->fileSystemManager();
    if (action == Qt::CopyAction)
        fsManager->copy(paths, destination);
    else if (action == Qt::MoveAction)
        fsManager->move(paths, destination);
    else if (action == Qt::LinkAction)
        fsManager->link(paths, destination);
}

/*!
 \internal

 Creates UI objects.
*/
void FileManagerEditor::setupUi()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    FileSystemModel *fsModel = pm->object<FileSystemModel>("FileSystemModel");
    NavigationModel *nModel = pm->object<NavigationModel>("navigationModel");
    connect(nModel, SIGNAL(pathsDropped(QString,QStringList,Qt::DropAction)),
            SLOT(onPathsDropped(QString,QStringList,Qt::DropAction)));

    splitter = new MiniSplitter(this);

    m_widget = new DualPaneWidget(fsModel,  splitter);
    m_widget->setContextMenuPolicy(Qt::CustomContextMenu);
    m_widget->setFocus();

    m_panel = new NavigationPanel(splitter);
    m_panel->setModel(nModel);

    splitter->addWidget(m_panel);
    splitter->addWidget(m_widget);
}

/*!
  \internal

  Connects UI objects to private slots.
*/
void FileManagerEditor::setupConnections()
{
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SLOT(onCurrentPathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SLOT(onOpenRequested(QString)));
    connect(m_widget, SIGNAL(selectedPathsChanged()), SLOT(onSelectedPathsChanged()));
    connect(m_widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));

    connect(m_panel, SIGNAL(triggered(QString)), m_widget, SLOT(setCurrentPath(QString)));

    connect(splitter, SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
  \internal

  Creates \a checkable action with \a text, connects it to DualPaneWidget's \a slot
  and registers it in ActionManager with \a id.
*/
QAction * FileManagerEditor::createAction(const QString &text, const QByteArray &id, const char *slot,
                                          bool checkable)
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

     QAction *action = new QAction(this);
     action->setText(text);
     action->setCheckable(checkable);
     if (!checkable)
         connect(action, SIGNAL(triggered()), m_widget, slot);
     else
         connect(action, SIGNAL(toggled(bool)), m_widget, slot);
     m_widget->addAction(action);
     actionManager->registerAction(action, id);
     return action;
}

/*!
  \internal

  Creates action with \a text for view mode \a mode.
*/
QAction * FileManagerEditor::createViewAction(const QString &text, const QByteArray &id, int mode)
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    QAction *action = new QAction(this);
    action->setText(text);
    action->setCheckable(true);
    viewModeGroup->addAction(action);

    viewModeMapper->setMapping(action, mode);
    connect(action, SIGNAL(toggled(bool)), viewModeMapper, SLOT(map()));

    m_widget->addAction(action);
    actionManager->registerAction(action, id);

    return action;
}

/*!
  \internal

  Creates all actions.
*/
void FileManagerEditor::createActions()
{
    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();

    openAction = createAction(tr("Open"), Constants::Actions::Open, SLOT(open()));
    newFolderAction = createAction(tr("New Folder"), Constants::Actions::NewFolder, SLOT(newFolder()));
    renameAction = createAction(tr("Rename"), Constants::Actions::Rename, SLOT(rename()));
    removeAction = createAction(tr("Remove"), Constants::Actions::Remove, SLOT(remove()));

    showFileInfoAction = new QAction(tr("File info"), this);
    connect(showFileInfoAction, SIGNAL(triggered()), this, SLOT(showFileInfo()));
    m_widget->addAction(showFileInfoAction);
    actionManager->registerAction(showFileInfoAction, Constants::Actions::FileInfo);

    redoAction = createAction(tr("Redo"), Constants::Actions::Redo, SLOT(redo()));
    undoAction = createAction(tr("Undo"), Constants::Actions::Undo, SLOT(undo()));
    redoAction->setEnabled(false);
    undoAction->setEnabled(false);
    connect(m_widget, SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)));
    connect(m_widget, SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)));

    cutAction = createAction(tr("Cut"), Constants::Actions::Cut, SLOT(cut()));
    copyAction = createAction(tr("Copy"), Constants::Actions::Copy, SLOT(copy()));
    pasteAction = createAction(tr("Paste"), Constants::Actions::Paste, SLOT(paste()));
    selectAllAction = createAction(tr("Select all"), Constants::Actions::SelectAll, SLOT(selectAll()));

//    actionManager->command(Constants::Actions::Up)->action(m_widget, SLOT(up()));

    showHiddenFilesAction = createAction(tr("Show hidden files"), Constants::Actions::ShowHiddenFiles,
                                         SLOT(showHiddenFiles(bool)), true);

    showLeftPanelAction = new QAction(tr("Show left panel"), this);
    showLeftPanelAction->setCheckable(true);
    this->addAction(showLeftPanelAction);
    connect(showLeftPanelAction, SIGNAL(toggled(bool)), this, SLOT(showLeftPanel(bool)));
    actionManager->registerAction(showLeftPanelAction, Constants::Actions::ShowLeftPanel);

    openAction->setEnabled(false);
    renameAction->setEnabled(false);
    removeAction->setEnabled(false);
    cutAction->setEnabled(false);
    copyAction->setEnabled(false);

    createViewActions();
}

/*!
  \internal

  Creates view actions.
*/
void FileManagerEditor::createViewActions()
{
    int viewMode = m_widget->viewMode();

    viewModeGroup = new QActionGroup(this);
    viewModeMapper = new QSignalMapper(this);

    iconModeAction = createViewAction(tr("Icon view"), Constants::Actions::IconMode, IconView);
    columnModeAction = createViewAction(tr("Column view"), Constants::Actions::ColumnMode, ColumnView);
    treeModeAction = createViewAction(tr("Tree view"), Constants::Actions::TreeMode, TreeView);
    coverFlowModeAction = createViewAction(tr("Cover flow"), Constants::Actions::CoverFlowMode, CoverFlow);
    dualPaneModeAction = createViewAction(tr("Dual pane"), Constants::Actions::DualPane, DualPane);

    viewModeGroup->addAction(dualPaneModeAction);

    switch (viewMode) {
    case IconView: iconModeAction->setChecked(true); break;
    case ColumnView: columnModeAction->setChecked(true); break;
    case TreeView: treeModeAction->setChecked(true); break;
    case CoverFlow: coverFlowModeAction->setChecked(true); break;
    case DualPane: coverFlowModeAction->setChecked(true); break;
    default: break;
    }
    dualPaneModeAction->setChecked(m_widget->dualPaneModeEnabled());

    connect(viewModeMapper, SIGNAL(mapped(int)), SLOT(setAndSaveViewMode(int)));
}

/*!
  \internal

  Restores FileManagerEditor's default settings.
*/
void FileManagerEditor::restoreDefaults()
{
    m_settings = Core::instance()->settings();

    int mode = 1;
    bool showLeftPanel = true;
    QVariantList lst;

    if (m_settings->contains(QLatin1String("fileManager/viewMode")))
        mode = m_settings->value(QLatin1String("fileManager/viewMode")).toInt();

    if (m_settings->contains(QLatin1String("fileManager/showLeftPanel")))
        showLeftPanel =  m_settings->value(QLatin1String("fileManager/showLeftPanel")).toBool();

    lst = m_settings->value(QLatin1String("fileManager/lastSplitterSizes")).toList();

    setViewMode(mode);

    m_panel->setVisible(showLeftPanel);

    if (!lst.isEmpty()) {
        splitter->setSizes(variantListToIntList(lst));
    } else {
        splitter->setSizes(QList<int>() << 200 << 600);
    }

    showLeftPanelAction->setChecked(showLeftPanel); // FIXME

    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/iconSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/gridSize"));
    m_settings->addObject(m_widget->leftWidget(), QLatin1String("fileManager/flow"));
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

