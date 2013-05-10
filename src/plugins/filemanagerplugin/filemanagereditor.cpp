#include "filemanagereditor.h"
#include "filemanagereditor_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtGui/QAction>
#include <QtGui/QDesktopServices>
#include <QtGui/QFileDialog>
#include <QtGui/QFileIconProvider>
#include <QtGui/QMenu>
#include <QtGui/QResizeEvent>
#include <QtGui/QToolBar>

#include <ExtensionSystem/PluginManager>
#include <GuiSystem/AbstractEditor>
#include <GuiSystem/EditorWindowFactory>
#include <GuiSystem/SharedProperties>
#include <GuiSystem/constants.h>
#include <Widgets/MiniSplitter>

#include <FileManager/FileExplorerWidget>
#include <FileManager/FileInfoDialog>
#include <FileManager/FileManagerHistory>
#include <FileManager/FileSystemManager>
#include <FileManager/FileSystemModel>
#include <FileManager/NavigationModel>
#include <FileManager/NavigationPanel>
#include <FileManager/constants.h>

#include "filemanagerdocument.h"
#include "filemanagerplugin.h"
#include "openwitheditormenu.h"

using namespace GuiSystem;
using namespace FileManager;

FileManagerEditorHistory::FileManagerEditorHistory(QObject *parent) :
    IHistory(parent),
    m_widget(0)
{
}

void FileManagerEditorHistory::setDualPaneWidget(FileManagerWidget *widget)
{
    m_widget = widget;
    connect(widget->history(), SIGNAL(currentItemIndexChanged(int)),
            this, SIGNAL(currentItemIndexChanged(int)));
}

void FileManagerEditorHistory::clear()
{
    m_widget->history()->clear();
}

void FileManagerEditorHistory::erase()
{
//    m_indexes.clear();
}

int FileManagerEditorHistory::count() const
{
    return m_widget->history()->count();
}

int FileManagerEditorHistory::currentItemIndex() const
{
    return m_widget->history()->currentItemIndex();
}

void FileManagerEditorHistory::setCurrentItemIndex(int index)
{
    m_widget->history()->setCurrentItemIndex(index);
}

HistoryItem FileManagerEditorHistory::itemAt(int index) const
{
    if (index < 0 || index >= count())
        return HistoryItem();

    FileManagerHistoryItem item;
    item = m_widget->history()->itemAt(index);
    HistoryItem result;
    result.setUrl(QUrl::fromLocalFile(item.path()));

    return result;
}

QByteArray FileManagerEditorHistory::store() const
{
    QByteArray history;
    QDataStream s(&history, QIODevice::WriteOnly);

    s << *(m_widget->history());

    return history;
}

void FileManagerEditorHistory::restore(const QByteArray &arr)
{
    QByteArray history(arr);
    QDataStream s(&history, QIODevice::ReadOnly);

    s >> *(m_widget->history());

    emit currentItemIndexChanged(currentItemIndex());
}

/*!
  \class FileManagerEditor
*/
FileManagerEditor::FileManagerEditor(QWidget *parent) :
    AbstractEditor(*new FileManagerDocument, parent),
    ignoreSignals(false)
{
    document()->setParent(this);
    setupUi();
    setupPropetries();
    setupConnections();
    createActions();
    retranslateUi();

    connectDocument(qobject_cast<FileManagerDocument *>(document()));
}

void FileManagerEditor::setDocument(AbstractDocument *document)
{
    if (this->document() == document)
        return;

    FileManagerDocument *fmDocument = qobject_cast<FileManagerDocument *>(document);
    if (!fmDocument)
        return;

    connectDocument(fmDocument);

    AbstractEditor::setDocument(document);
}

/*!
  \internal

  Restores FileManagerEditor's default settings.
*/
void FileManagerEditor::restoreDefaults()
{
    QVariant value = m_properties->value("splitterState");

    if (value.isValid()) {
        m_widget->splitter()->restoreState(value.toByteArray());
    } else {
        m_widget->splitter()->setSizes(QList<int>() << 200 << 600);
    }
}

/*!
  \reimp
*/
bool FileManagerEditor::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    bool ok = true;
    QByteArray baseState, widgetState;

    s >> baseState;
    s >> widgetState;

    ok |= AbstractEditor::restoreState(baseState);
    // we have to block signals to not rewrite user settings
    bool bs0 = m_widget->blockSignals(true);
    bool bs1 = m_widget->splitter()->blockSignals(true);
    bool bs2 = m_widget->widget()->blockSignals(true);
    bool bs3 = m_widget->statusBar()->blockSignals(true);
    ok |= m_widget->restoreState(widgetState);
    m_widget->blockSignals(bs0);
    m_widget->splitter()->blockSignals(bs1);
    m_widget->widget()->blockSignals(bs2);
    m_widget->statusBar()->blockSignals(bs3);

    return ok;
}

/*!
  \reimp
*/
QByteArray FileManagerEditor::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << AbstractEditor::saveState();
    s << m_widget->saveState();

    return state;
}

/*!
  \reimp
*/
void FileManagerEditor::resizeEvent(QResizeEvent *e)
{
    m_widget->resize(e->size());
}

void FileManagerEditor::onSelectedPathsChanged()
{
    QStringList paths = m_widget->widget()->selectedPaths();;
    bool enabled = !paths.empty();

    m_openTabAction->setEnabled(enabled);
    m_openWindowAction->setEnabled(enabled);
    m_openEditorAction->setEnabled(enabled);
}

/*!
    \internal
*/
void FileManagerEditor::onSortingChanged()
{
    int sortColumn = m_widget->widget()->sortingColumn();
    int sortOrder = m_widget->widget()->sortingOrder();

    m_properties->setValue("sortingColumn", sortColumn);
    m_properties->setValue("sortingOrder", sortOrder);
}

/*!
    \internal
*/
void FileManagerEditor::onSplitterMoved(int, int)
{
    m_properties->setValue("splitterState", m_widget->splitter()->saveState());
}

/*!
    \internal
*/
void FileManagerEditor::openPaths(const QStringList &paths,Qt::KeyboardModifiers modifiers)
{
    QStringList folders;
    foreach (const QString &path, paths) {
        QFileInfo info(path);
        if (info.isDir() && !info.isBundle()) {
            folders.append(path);
        } else {
#ifdef Q_OS_LINUX
            QFileInfo info(path);
            if (info.isExecutable()) {
                QProcess::startDetached(path);
                return;
            }
#endif
            // TODO: allow to open default editor instead
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }

    if (folders.isEmpty())
        return;

    QList<QUrl> urls;
    foreach (const QString &path, folders) {
        urls.append(QUrl::fromLocalFile(path));
    }
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        if (modifiers == Qt::NoModifier)
            factory->open(urls);
        else if (modifiers == (Qt::ControlModifier | Qt::AltModifier))
            factory->openNewWindow(urls);
        else if (modifiers == (Qt::ControlModifier))
            factory->openNewEditor(urls);
    }
}

/*!
    \internal
*/
void FileManagerEditor::openNewTab()
{
    QStringList paths = m_widget->widget()->selectedPaths();
    if (paths.isEmpty())
        return;

    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->openNewEditor(urls);
}

/*!
    \internal
*/
void FileManagerEditor::openNewWindow()
{
    QStringList paths = m_widget->widget()->selectedPaths();
    if (paths.isEmpty())
        return;

    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->openNewWindow(urls);
}

void FileManagerEditor::openEditor()
{
    QStringList paths = m_widget->widget()->selectedPaths();
    if (paths.isEmpty())
        return;

    QList<QUrl> urls;
    foreach (const QString &path, paths) {
        urls.append(QUrl::fromLocalFile(path));
    }

    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->open(urls);
}

void FileManagerEditor::openEditor(const QList<QUrl> &urls, const QByteArray &editor)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (!factory)
        return;

    factory->openEditor(urls, editor);
}

void FileManagerEditor::showContextMenu(const QPoint &pos)
{
    FileManagerWidget *widget = qobject_cast<FileManagerWidget *>(sender());
    Q_ASSERT(widget);

    QStringList paths = widget->selectedPaths();
    QMenu *menu = widget->createStandardMenu(paths);
    QList<QAction *> actions = menu->actions();

    if (!paths.isEmpty()) {
        QAction *actionBefore = actions.at(1);

        menu->insertAction(actionBefore, m_openTabAction);
        menu->insertAction(actionBefore, m_openWindowAction);

        OpenWithEditorMenu *openWithMenu = new OpenWithEditorMenu(menu);
        openWithMenu->setPaths(paths);
        connect(openWithMenu, SIGNAL(openRequested(QList<QUrl>,QByteArray)), SLOT(openEditor(QList<QUrl>,QByteArray)));

        if (!openWithMenu->isEmpty()) {
            menu->insertSeparator(actionBefore);
            menu->insertAction(actionBefore, m_openEditorAction);
            if (openWithMenu->actions().count() > 1)
                menu->insertMenu(actionBefore, openWithMenu);
        }
    }

    menu->exec(widget->mapToGlobal(pos));
    delete menu;
}

/*!
    \internal
*/
void FileManagerEditor::setupUi()
{
    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    NavigationModel *model = pm->object<NavigationModel>("navigationModel");

    m_widget = new FileExplorerWidget(model, this);
}

/*!
    \internal
*/
void FileManagerEditor::setupPropetries()
{
    m_properties = FileManagerPlugin::instance()->properties();

    FileManagerWidget *widget = m_widget->widget();
    m_properties->addProperty("flow", widget);
    m_properties->addProperty("gridSize", widget);
    m_properties->addProperty("iconSize", widget);
    m_properties->addProperty("iconSizeColumn", widget);
    m_properties->addProperty("iconSizeTree", widget);
    m_properties->addProperty("itemsExpandable", widget);
    m_properties->addProperty("sortingColumn", widget);
    m_properties->addProperty("sortingOrder", widget);
    m_properties->addProperty("viewMode", widget);

    m_properties->addProperty("panelVisible", m_widget);
    m_properties->addProperty("statusBarVisible", m_widget);
}

/*!
    \internal
*/
void FileManagerEditor::setupConnections()
{
    FileManagerWidget *widget = m_widget->widget();
    connect(widget, SIGNAL(selectedPathsChanged()), SLOT(onSelectedPathsChanged()));
    connect(widget, SIGNAL(openRequested(QStringList,Qt::KeyboardModifiers)),
            this, SLOT(openPaths(QStringList,Qt::KeyboardModifiers)));
    connect(widget, SIGNAL(sortingChanged()), SLOT(onSortingChanged()));

    connect(m_widget->panel(), SIGNAL(triggered(QString)), widget, SLOT(setCurrentPath(QString)));

    connect(m_widget->splitter(), SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
    \internal
*/
void FileManagerEditor::createActions()
{
    m_openTabAction = new QAction(this);
    m_openTabAction->setEnabled(false);
    m_openTabAction->setObjectName(Constants::Actions::OpenInTab);
    connect(m_openTabAction, SIGNAL(triggered()), SLOT(openNewTab()));
    addAction(m_openTabAction);

    m_openWindowAction = new QAction(this);
    m_openWindowAction->setEnabled(false);
    m_openWindowAction->setObjectName(Constants::Actions::OpenInWindow);
    connect(m_openWindowAction, SIGNAL(triggered()), SLOT(openNewWindow()));
    addAction(m_openWindowAction);

    m_openEditorAction = new QAction(this);
    m_openEditorAction->setEnabled(false);
    m_openEditorAction->setObjectName(Constants::Actions::OpenEditor);
    connect(m_openEditorAction, SIGNAL(triggered()), SLOT(openEditor()));
    addAction(m_openEditorAction);

    registerWidgetActions(m_widget->widget());
}

void FileManagerEditor::retranslateUi()
{
    m_openTabAction->setText(tr("Open in new tab"));
    m_openWindowAction->setText(tr("Open in new window"));
    m_openEditorAction->setText(tr("Open in internal editor"));
    m_openEditorAction->setToolTip(tr("Opens selected files in an internal editor"));
}

void FileManagerEditor::registerWidgetActions(FileManagerWidget *widget)
{
    widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
}

void FileManagerEditor::connectDocument(FileManagerDocument *document)
{
    Q_ASSERT(document);

    document->fmhistory()->setDualPaneWidget(m_widget->widget());

    connect(document, SIGNAL(currentPathChanged(QString)),
            m_widget->widget(), SLOT(setCurrentPath(QString)));
    connect(m_widget->widget(), SIGNAL(currentPathChanged(QString)),
            document, SLOT(setCurrentPath(QString)));
}

/*!
    \class FileManagerEditorFactory
*/

FileManagerEditorFactory::FileManagerEditorFactory(QObject *parent) :
    AbstractEditorFactory("FileManager", parent)
{
}

/*!
    \reimp
*/
AbstractEditor * FileManagerEditorFactory::createEditor(QWidget *parent)
{
    return new FileManagerEditor(parent);
}
