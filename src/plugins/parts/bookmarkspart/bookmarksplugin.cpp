#include "bookmarksplugin.h"
#include "bookmarksplugin_p.h"

#include <QtCore/QtPlugin>
#include <QtCore/QSettings>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidgetAction>
#else
#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>
#endif

#include <ExtensionSystem/PluginManager>

#include <Parts/AbstractDocument>
#include <Parts/EditorWindow>
#include <Parts/EditorWindowFactory>
#include <Parts/ContextCommand>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>
#include <Parts/Separator>
#include <Parts/ToolWidgetManager>
#include <Parts/constants.h>

#include <Bookmarks/BookmarksDialog>
#include <Bookmarks/BookmarksModel>

#include "bookmarksconstants.h"
#include "bookmarksdocument.h"
#include "bookmarkseditor.h"
#include "bookmarkstoolwidget.h"

using namespace ExtensionSystem;
using namespace Parts;
using namespace Bookmarks;

BookmarksButtonCommand::BookmarksButtonCommand(QObject *parent) :
    AbstractCommand("Bookmarks", parent)
{
}

QAction * BookmarksButtonCommand::createAction(QObject *parent) const
{
    QToolButton *button = new QToolButton;
    button->setIcon(QIcon(":/bookmarks/icons/bookmarks.png"));
    button->setToolTip(tr("Show bookmarks"));
    connect(button, SIGNAL(clicked()), BookmarksPlugin::instance(), SLOT(showBookmarks()));

    QWidgetAction *action = new QWidgetAction(parent);
    action->setDefaultWidget(button);
    return action;
}

BookmarksToolBarContainer::BookmarksToolBarContainer(const QByteArray &id, QObject *parent) :
    ModelContainer(id, parent)
{
    BookmarksModel *model = BookmarksPlugin::instance()->model();
    setModel(model, model->toolBar());
}

static BookmarksPlugin *m_instance = 0;
BookmarksPlugin::BookmarksPlugin() :
    ExtensionSystem::IPlugin()
{
    m_instance = this;
}

BookmarksPlugin * BookmarksPlugin::instance()
{
    return m_instance;
}

bool BookmarksPlugin::initialize()
{
    m_document = new BookmarksDocument(this);

    m_model = m_document->model();
//    addObject(model, QLatin1String(Constants::Objects::BookmarksModel));

    if (!m_model->loadBookmarks()) {
        addDefaultBookmarks();
    }

    DocumentManager::instance()->addFactory(new BookmarksDocumentFactory(this));
    EditorManager::instance()->addFactory(new BookmarksEditorFactory(this));
    ToolWidgetManager::instance()->addFactory(new BookmarksToolWidgetFactory(this));

    createActions();

    return true;
}

void BookmarksPlugin::shutdown()
{
    m_model->saveBookmarks();
}

BookmarksDocument * BookmarksPlugin::sharedDocument() const
{
    return m_document;
}

void BookmarksPlugin::open(const QUrl &url)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->open(url);
    }
}

void BookmarksPlugin::openInTabs(const QList<QUrl> &urls)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->openNewEditor(urls);
    }
}

void BookmarksPlugin::openInWindow(const QList<QUrl> &urls)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->openNewWindow(urls);
    }
}

void BookmarksPlugin::onToolBarTriggered(const QModelIndex &index)
{
    open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksPlugin::onBookmarksButtonTriggered()
{

}

void BookmarksPlugin::showBookmarks()
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->openEditor(Constants::Editors::Bookmarks);
    }
}

void BookmarksPlugin::addBookmark()
{
    showBookmarkDialog(QModelIndex(), false);
}

void BookmarksPlugin::addFolder()
{
    showBookmarkDialog(QModelIndex(), true);
}

void BookmarksPlugin::createActions()
{
    BookmarksToolBarContainer * toolBar = new BookmarksToolBarContainer("AlternateToolbar", this);
    toolBar->addCommand(new BookmarksButtonCommand(toolBar), toolBar->firstSeparator());
    connect(toolBar, SIGNAL(triggered(QModelIndex)), this, SLOT(onToolBarTriggered(QModelIndex)));

    addBookmarkAction = new QAction(tr("Add bookmark"), this);
    addBookmarkAction->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    connect(addBookmarkAction, SIGNAL(triggered()), SLOT(addBookmark()));

    addFolderAction = new QAction(tr("Add folder"), this);
    addFolderAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Alt+F")));
    connect(addFolderAction, SIGNAL(triggered()), SLOT(addFolder()));

    showBookmarksAction = new QAction(tr("Show bookmarks"), this);
    showBookmarksAction->setShortcut(QKeySequence(QLatin1String("Alt+Ctrl+B")));
    connect(showBookmarksAction, SIGNAL(triggered()), SLOT(showBookmarks()));

    QList<QAction*> actions;
    actions.append(addBookmarkAction);
    actions.append(addFolderAction);
    actions.append(showBookmarksAction);

    // ================ View Menu ================
    ContextCommand *c = new ContextCommand(Constants::Actions::ShowBookmarks, this);
    c->setShortcut(QKeySequence());
    c->setText(tr("Show Bookmarks toolbar"));
    c->setAttributes(ContextCommand::AttributeUpdateEnabled);

    // ================ Bookmarks Menu ================
    BookmarksMenuContainer *menu = new BookmarksMenuContainer(Constants::Menus::Bookmarks, this);
    menu->setText(tr("Bookmarks"));
    menu->bookmarksMenu()->setInitialActions(actions);
    menu->bookmarksMenu()->setModel(m_model);
    connect(menu->bookmarksMenu(), SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(menu->bookmarksMenu(), SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(menu->bookmarksMenu(), SIGNAL(openInWindow(QList<QUrl>)), SLOT(openInWindow(QList<QUrl>)));
    addObject(menu);
}

void BookmarksPlugin::showBookmarkDialog(const QModelIndex &index, bool isFolder)
{
    EditorWindow * window = EditorWindow::currentWindow();
    if (!window)
        return;

    AbstractEditor *editor = window->editor();
    if (!editor) // paranoia
        return;

    AbstractDocument *document = editor->document();

    BookmarkDialog dialog;
    dialog.setModel(m_model);
    dialog.setFolder(isFolder);
    dialog.showUrl(false);
    dialog.setCurrentIndex(index);
    if (!isFolder) {
        dialog.setTitle(document->title());
        dialog.setUrl(document->url().toString());
        dialog.setIcon(document->icon());
//        dialog.setPreview(document->preview());
    } else {
        dialog.setTitle(tr("New folder"));
    }
    dialog.exec();
}

static Bookmark bookmark(const QString &title, const QUrl &url)
{
    Bookmark b;
    b.setTitle(title);
    b.setUrl(url);
    b.setIcon(QFileIconProvider().icon(QFileIconProvider::Network));
    return b;
}

void BookmarksPlugin::addDefaultBookmarks()
{
    QModelIndex toolBar = m_model->toolBar();
    m_model->addBookmark(bookmark("Andromeda", QUrl("http://gitorious.org/andromeda/pages/Home")), toolBar);
    m_model->addBookmark(bookmark("Google", QUrl("http://google.com")), toolBar);
    m_model->addBookmark(bookmark("YouTube", QUrl("http://www.youtube.com/")), toolBar);
    m_model->addBookmark(bookmark("Wikipedia", QUrl("http://www.wikipedia.org/")), toolBar);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN(BookmarksPlugin)
#endif
