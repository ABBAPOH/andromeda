#include "filemanagereditor.h"
#include "filemanagereditor_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QProcess>
#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtGui/QDesktopServices>
#include <QtGui/QResizeEvent>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolBar>
#else
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <QtGui/QFileIconProvider>
#include <QtGui/QMenu>
#include <QtGui/QToolBar>
#endif

#include <ExtensionSystem/PluginManager>
#include <Parts/AbstractEditor>
#include <Parts/OpenStrategy>
#include <Parts/SharedProperties>
#include <Parts/constants.h>
#include <Widgets/MiniSplitter>

#include <FileManager/FileExplorerWidget>
#include <FileManager/FileInfoDialog>
#include <FileManager/FileManagerHistory>
#include <FileManager/FileManagerModel>
#include <FileManager/FileSystemManager>
#include <FileManager/FileSystemModel>
#include <FileManager/NavigationModel>
#include <FileManager/NavigationPanel>
#include <FileManager/constants.h>

#include "filemanagerdocument.h"
#include "filemanagerplugin.h"
#include "openwitheditormenu.h"

using namespace Parts;
using namespace FileManager;

FileManagerEditorHistory::FileManagerEditorHistory(QObject *parent) :
    IHistory(parent),
    m_history(0)
{
}

void FileManagerEditorHistory::setHistory(FileManagerHistory *history)
{
    Q_ASSERT(!m_history);
    Q_ASSERT(history);

    m_history = history;
    connect(m_history, SIGNAL(currentItemIndexChanged(int)),
            this, SIGNAL(currentItemIndexChanged(int)));
}

void FileManagerEditorHistory::clear()
{
    m_history->clear();
}

void FileManagerEditorHistory::erase()
{
//    m_indexes.clear();
}

int FileManagerEditorHistory::count() const
{
    return m_history->count();
}

int FileManagerEditorHistory::currentItemIndex() const
{
    return m_history->currentItemIndex();
}

void FileManagerEditorHistory::setCurrentItemIndex(int index)
{
    m_history->setCurrentItemIndex(index);
}

HistoryItem FileManagerEditorHistory::itemAt(int index) const
{
    if (index < 0 || index >= count())
        return HistoryItem();

    FileManagerHistoryItem item;
    item = m_history->itemAt(index);
    HistoryItem result;
    result.setUrl(item.url());

    return result;
}

QByteArray FileManagerEditorHistory::store() const
{
    QByteArray history;
    QDataStream s(&history, QIODevice::WriteOnly);

    s << *(m_history);

    return history;
}

void FileManagerEditorHistory::restore(const QByteArray &arr)
{
    QByteArray history(arr);
    QDataStream s(&history, QIODevice::ReadOnly);

    s >> *(m_history);

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
    QList<QUrl> urls = m_widget->widget()->selectedUrls();;

    foreach (const StrategyAction &action, strategyActions) {
        bool enabled = action.first->canOpen(urls);
        action.second->setEnabled(enabled);
    }
}

/*!
    \internal
*/
void FileManagerEditor::onSortingChanged()
{
    int sortColumn = m_widget->widget()->model()->sortColumn();
    int sortOrder = m_widget->widget()->model()->sortOrder();

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
void FileManagerEditor::openPaths(const QList<QUrl> &urls, Qt::KeyboardModifiers modifiers)
{
    QList<QUrl> folders;
    foreach (const QUrl &url, urls) {
        Q_ASSERT(url.isLocalFile());
        QString path = url.toLocalFile();
        QFileInfo info(path);
        if (info.isDir() && !info.isBundle()) {
            folders.append(url);
        } else {
#ifdef Q_OS_LINUX
            if (info.isExecutable()) {
                QProcess::startDetached(path);
                return;
            }
#endif
            // TODO: allow to open default editor instead
            QDesktopServices::openUrl(url);
        }
    }

    if (folders.isEmpty())
        return;

    OpenStrategy *strategy = OpenStrategy::strategy(modifiers);
    if (strategy)
        strategy->open(folders);
}

void FileManagerEditor::openStrategy()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    QList<QUrl> urls = m_widget->widget()->selectedUrls();
    if (urls.isEmpty())
        return;

    QByteArray id = action->objectName().toUtf8();
    OpenStrategy *strategy = OpenStrategy::strategy(id);
    if (strategy)
        strategy->open(urls);
}

void FileManagerEditor::showContextMenu(const QPoint &pos)
{
    FileManagerWidget *widget = qobject_cast<FileManagerWidget *>(sender());
    Q_ASSERT(widget);

    QList<QUrl> urls = widget->selectedUrls();
    QMenu *menu = widget->createStandardMenu(urls);
    QList<QAction *> actions = menu->actions();

    if (!urls.isEmpty()) {
        QAction *actionBefore = actions.at(1);
        menu->removeAction(actions.at(0));

        foreach (const StrategyAction &action, strategyActions)
            menu->insertAction(actionBefore, action.second);
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
    NavigationModel *model = pm->objectPool()->object<NavigationModel>("navigationModel");

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
    m_properties->addProperty("sortColumn", widget->model());
    m_properties->addProperty("sortOrder", widget->model());
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
    connect(widget, SIGNAL(openRequested(QList<QUrl>,Qt::KeyboardModifiers)),
            this, SLOT(openPaths(QList<QUrl>,Qt::KeyboardModifiers)));
    connect(widget->model(), SIGNAL(sortingChanged(int,Qt::SortOrder)), SLOT(onSortingChanged()));

    connect(m_widget->splitter(), SIGNAL(splitterMoved(int,int)), SLOT(onSplitterMoved(int,int)));
}

/*!
    \internal
*/
void FileManagerEditor::createActions()
{
    foreach (OpenStrategy *strategy, OpenStrategy::strategies()) {
        QAction *action = new QAction(this);
        action->setShortcut(strategy->keySequence());
        action->setText(strategy->text());
        action->setToolTip(strategy->toolTip());
        action->setObjectName(strategy->id());
        action->setEnabled(false);
        connect(action, SIGNAL(triggered()), SLOT(openStrategy()));
        addAction(action);
        strategyActions.append(qMakePair(strategy, action));
    }

    registerWidgetActions(m_widget->widget());
}

void FileManagerEditor::registerWidgetActions(FileManagerWidget *widget)
{
    widget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(widget, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
}

void FileManagerEditor::connectDocument(FileManagerDocument *document)
{
    Q_ASSERT(document);

    document->fmhistory()->setHistory(m_widget->widget()->model()->history());

    connect(document, SIGNAL(urlChanged(QUrl)),
            m_widget->widget(), SLOT(setUrl(QUrl)));
    connect(m_widget->widget(), SIGNAL(urlChanged(QUrl)),
            document, SLOT(setUrl(QUrl)));
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
