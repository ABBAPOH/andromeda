#include "bookmarksplugin.h"
#include "bookmarkseditor.h"

#include "bookmarksconstants.h"

#include <QtCore/QtPlugin>
#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>

#include <extensionsystem/pluginmanager.h>

#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/editormanager.h>

#include <bookmarks/bookmarkdialog.h>
#include <bookmarks/bookmarksmenu.h>
#include <bookmarks/bookmarksmodel.h>
#include <bookmarks/bookmarkstoolbar.h>
#include <bookmarks/bookmarkswidget.h>

#include <coreplugin/core.h>
#include <coreplugin/constants.h>
#include <coreplugin/mainwindow.h>

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace Bookmarks;
using namespace CorePlugin;
using namespace BookmarksPlugin;

class BookmarksMenuContainer : public CommandContainer
{
public:
    explicit BookmarksMenuContainer(const QByteArray &id, QObject *parent = 0) :
        CommandContainer(id, parent),
        m_menu(new BookmarksMenuBarMenu)
    {}

    ~BookmarksMenuContainer() { delete m_menu; }

    QMenu *createMenu() const { return m_menu; }
    inline BookmarksMenuBarMenu *bookmarksMenu() const { return m_menu; }

private:
    BookmarksMenuBarMenu *m_menu;
};

BookmarksToolBarContainer::~BookmarksToolBarContainer()
{
    qDeleteAll(toolBars);
}

QToolBar *BookmarksToolBarContainer::createToolBar() const
{
    PluginManager *manager = PluginManager::instance();
    BookmarksModel *model = manager->object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));

    BookmarksToolBar *toolBar = new BookmarksToolBar();
    toolBar->setObjectName(QLatin1String("bookmarksToolbar"));
    toolBar->setModel(model);
    toolBar->setRootIndex(model->toolBar());
    toolBar->show();

    connect(toolBar, SIGNAL(open(QUrl)), SIGNAL(open(QUrl)));
    connect(toolBar, SIGNAL(openInTabs(QList<QUrl>)), SIGNAL(openInTabs(QList<QUrl>)));
    connect(toolBar, SIGNAL(addBookmarkTriggered()), SIGNAL(addBookmarkTriggered()));
    connect(toolBar, SIGNAL(addFolderTriggered()), SIGNAL(addFolderTriggered()));

    QList<QAction*> actions;
    QAction *a = new QAction(QIcon(":/icons/bookmarks.png"), QString(), toolBar);
    a->setToolTip(tr("Show bookmarks"));
    connect(a, SIGNAL(triggered()), SIGNAL(showBookmarksTriggered()));
    actions.append(a);
    toolBar->setInitialActions(actions);
    connect(toolBar, SIGNAL(destroyed(QObject*)), SLOT(onDestroy(QObject*)));
    const_cast<BookmarksToolBarContainer*>(this)->toolBars.append(toolBar);
    return toolBar;
}

void BookmarksToolBarContainer::onDestroy(QObject *o)
{
    toolBars.removeAll(o);
}

BookmarksPluginImpl::BookmarksPluginImpl() :
    ExtensionSystem::IPlugin()
{
}

bool BookmarksPluginImpl::initialize(const QVariantMap &)
{
    BookmarksEditorFactory *f = new BookmarksEditorFactory(this);
    EditorManager::instance()->addFactory(f);

    model = new BookmarksModel(this);
    addObject(model, QLatin1String(Constants::Objects::BookmarksModel));

    if (!model->loadBookmarks()) {
        addDefaultBookmarks();
    }

    createActions();

    return true;
}

void BookmarksPluginImpl::shutdown()
{
    model->saveBookmarks();
}

void BookmarksPluginImpl::open(const QUrl &url)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    window->open(url);
}

void BookmarksPluginImpl::openInTabs(const QList<QUrl> &urls)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    for (int i = 0; i < urls.size(); i++) {
        window->openNewTab(urls.at(i));
    }
}

void BookmarksPluginImpl::openInWindow(const QList<QUrl> &urls)
{
    MainWindow * window = MainWindow::createWindow();

    for (int i = 0; i < urls.size(); i++) {
        window->openNewTab(urls.at(i));
    }
    window->show();
}

void BookmarksPluginImpl::showBookmarks()
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    window->openEditor(QLatin1String(Constants::Editors::Bookmarks));
}

void BookmarksPluginImpl::addBookmark()
{
    showBookmarkDialog(QModelIndex(), false);
}

void BookmarksPluginImpl::addFolder()
{
    showBookmarkDialog(QModelIndex(), true);
}

void BookmarksPluginImpl::createActions()
{
    BookmarksModel *model = object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));

    ActionManager *actionManager = ActionManager::instance();
    CommandContainer *menuBarContainer = actionManager->container(Constants::Menus::MenuBar);

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

    // ================ Bookmarks Menu ================
    BookmarksMenuContainer *menu = new BookmarksMenuContainer(Constants::Menus::Bookmarks, this);
    menu->setTitle(tr("Bookmarks"));
    menu->bookmarksMenu()->setInitialActions(actions);
    menu->bookmarksMenu()->setModel(model);
    connect(menu->bookmarksMenu(), SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(menu->bookmarksMenu(), SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(menu->bookmarksMenu(), SIGNAL(openInWindow(QList<QUrl>)), SLOT(openInWindow(QList<QUrl>)));
    menuBarContainer->addContainer(menu);

    BookmarksToolBarContainer *toolBar = new BookmarksToolBarContainer(Constants::Objects::AlternateToolbar, this);
    connect(toolBar, SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(toolBar, SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(toolBar, SIGNAL(showBookmarksTriggered()), SLOT(showBookmarks()));
    connect(toolBar, SIGNAL(addBookmarkTriggered()), SLOT(addBookmark()));
    connect(toolBar, SIGNAL(addFolderTriggered()), SLOT(addFolder()));
}

void BookmarksPluginImpl::showBookmarkDialog(const QModelIndex &index, bool isFolder)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    AbstractEditor *editor = window->currentEditor();
    if (!editor) // paranoia
        return;

    BookmarksModel *model = object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));

    BookmarkDialog dialog;
    dialog.setModel(model);
    dialog.setFolder(isFolder);
    dialog.showUrl(false);
    dialog.setCurrentIndex(index);
    if (!isFolder) {
        dialog.setTitle(editor->title());
        dialog.setUrl(editor->url().toString());
        dialog.setIcon(editor->icon());
        dialog.setPreview(editor->preview());
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

void BookmarksPluginImpl::addDefaultBookmarks()
{
    QModelIndex toolBar = model->toolBar();
    model->addBookmark(bookmark("Andromeda", QUrl("http://gitorious.org/andromeda/pages/Home")), toolBar);
    model->addBookmark(bookmark("Google", QUrl("http://google.com")), toolBar);
    model->addBookmark(bookmark("YouTube", QUrl("http://www.youtube.com/")), toolBar);
    model->addBookmark(bookmark("Wikipedia", QUrl("http://www.wikipedia.org/")), toolBar);
}

Q_EXPORT_PLUGIN(BookmarksPluginImpl)
