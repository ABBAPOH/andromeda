#include "bookmarksplugin.h"
#include "bookmarkseditor.h"

#include <QtCore/QtPlugin>
#include <QtCore/QSettings>

#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>

#include <extensionsystem/pluginmanager.h>

#include <guisystem/abstractcontainer.h>
#include <guisystem/actionmanager.h>
#include <guisystem/mainwindow.h>
#include <guisystem/command.h>
#include <guisystem/editormanager.h>

#include <core/constants.h>

#include "bookmarksconstants.h"
#include "bookmarkdialog.h"
#include "bookmarksmenu.h"
#include "bookmarksmodel.h"
#include "bookmarkstoolbar.h"
#include "bookmarkswidget.h"

using namespace ExtensionSystem;
using namespace GuiSystem;
using namespace Bookmarks;

class BookmarksMenuContainer : public CommandContainer
{
public:
    explicit BookmarksMenuContainer(const QByteArray &id, QObject *parent = 0) :
        CommandContainer(id, parent),
        m_menu(new BookmarksMenuBarMenu)
    {}

    ~BookmarksMenuContainer() { delete m_menu; }

    QMenu *createMenu(QWidget * = 0) const { return m_menu; }
    inline BookmarksMenuBarMenu *bookmarksMenu() const { return m_menu; }

private:
    BookmarksMenuBarMenu *m_menu;
};

BookmarksToolBarContainer::~BookmarksToolBarContainer()
{
    QList<QObject *> toolBars = this->toolBars;
    qDeleteAll(toolBars);
}

QToolBar *BookmarksToolBarContainer::createToolBar(QWidget *parent) const
{
    ActionManager *am = ActionManager::instance();

    PluginManager *manager = PluginManager::instance();
    BookmarksModel *model = manager->object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));

    BookmarksToolBar *toolBar = new BookmarksToolBar(parent);
    toolBar->setObjectName(QLatin1String("bookmarksToolbar"));
    toolBar->setModel(model);
    toolBar->setRootIndex(model->toolBar());

    QSettings settings;
    settings.beginGroup(QLatin1String("bookmarks"));
    bool visible = settings.value(QLatin1String("toolbarVisible"), true).toBool();
    toolBar->setVisible(visible);

    if (parent) {
        QAction *act = new QAction(tr("Show bookmarks toolbar"), parent);
        parent->addAction(act);
        act->setCheckable(true);
        act->setChecked(visible);
        connect(act, SIGNAL(triggered(bool)), toolBar, SLOT(setVisible(bool)));
        connect(act, SIGNAL(triggered(bool)), this, SLOT(storeVisibility(bool)));
        am->registerAction(act, Constants::Actions::ShowBookmarks);
    }

    connect(toolBar, SIGNAL(open(QUrl)), SIGNAL(open(QUrl)));
    connect(toolBar, SIGNAL(openInTabs(QList<QUrl>)), SIGNAL(openInTabs(QList<QUrl>)));
    connect(toolBar, SIGNAL(addBookmarkTriggered()), SIGNAL(addBookmarkTriggered()));
    connect(toolBar, SIGNAL(addFolderTriggered()), SIGNAL(addFolderTriggered()));

    QToolButton *button = new QToolButton(toolBar);
    button->setIcon(QIcon(":/icons/bookmarks.png"));
    button->setToolTip(tr("Show bookmarks"));
    connect(button, SIGNAL(clicked()), SIGNAL(showBookmarksTriggered()));

    QWidgetAction *action = new QWidgetAction(toolBar);
    action->setDefaultWidget(button);

    QList<QAction*> actions;
    actions.append(action);
    toolBar->setInitialActions(actions);
    connect(toolBar, SIGNAL(destroyed(QObject*)), SLOT(onDestroy(QObject*)));
    const_cast<BookmarksToolBarContainer*>(this)->toolBars.append(toolBar);
    return toolBar;
}

void BookmarksToolBarContainer::onDestroy(QObject *o)
{
    toolBars.removeAll(o);
}

BookmarksPlugin::BookmarksPlugin() :
    ExtensionSystem::IPlugin()
{
}

bool BookmarksPlugin::initialize(const QVariantMap &)
{
    model = new BookmarksModel(this);
    addObject(model, QLatin1String(Constants::Objects::BookmarksModel));

    if (!model->loadBookmarks()) {
        addDefaultBookmarks();
    }

    BookmarksEditorFactory *f = new BookmarksEditorFactory(this);
    addObject(f);
    EditorManager::instance()->addFactory(f);

    createActions();

    return true;
}

void BookmarksPlugin::shutdown()
{
    model->saveBookmarks();
}

void BookmarksPlugin::open(const QUrl &url)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    window->open(url);
}

void BookmarksPlugin::openInTabs(const QList<QUrl> &urls)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    for (int i = 0; i < urls.size(); i++) {
        window->openNewEditor(urls.at(i));
    }
}

void BookmarksPlugin::openInWindow(const QList<QUrl> &urls)
{
    MainWindow * window = MainWindow::createWindow();

    for (int i = 0; i < urls.size(); i++) {
        window->openNewEditor(urls.at(i));
    }
    window->show();
}

void BookmarksPlugin::showBookmarks()
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    window->openEditor(QLatin1String(Constants::Editors::Bookmarks));
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

    // ================ View Menu ================
    CommandContainer *viewMenu = actionManager->container(Constants::Menus::View);
    Command *c = new Command(Constants::Actions::ShowBookmarks, QKeySequence(), tr("Show Bookmarks toolbar"), this);
    c->setContext(Command::WindowCommand);
    viewMenu->addCommand(c);

    // ================ Bookmarks Menu ================
    BookmarksMenuContainer *menu = new BookmarksMenuContainer(Constants::Menus::Bookmarks, this);
    menu->setTitle(tr("Bookmarks"));
    menu->bookmarksMenu()->setInitialActions(actions);
    menu->bookmarksMenu()->setModel(model);
    connect(menu->bookmarksMenu(), SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(menu->bookmarksMenu(), SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(menu->bookmarksMenu(), SIGNAL(openInWindow(QList<QUrl>)), SLOT(openInWindow(QList<QUrl>)));
    menuBarContainer->addContainer(menu, "035");
    addObject(menu);

    BookmarksToolBarContainer *toolBar = new BookmarksToolBarContainer(Constants::Objects::AlternateToolbar, this);
    connect(toolBar, SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(toolBar, SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(toolBar, SIGNAL(showBookmarksTriggered()), SLOT(showBookmarks()));
    connect(toolBar, SIGNAL(addBookmarkTriggered()), SLOT(addBookmark()));
    connect(toolBar, SIGNAL(addFolderTriggered()), SLOT(addFolder()));
    addObject(toolBar);
}

void BookmarksPlugin::showBookmarkDialog(const QModelIndex &index, bool isFolder)
{
    MainWindow * window = MainWindow::currentWindow();
    if (!window)
        return;

    AbstractEditor *editor = window->contanier();
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

void BookmarksPlugin::addDefaultBookmarks()
{
    QModelIndex toolBar = model->toolBar();
    model->addBookmark(bookmark("Andromeda", QUrl("http://gitorious.org/andromeda/pages/Home")), toolBar);
    model->addBookmark(bookmark("Google", QUrl("http://google.com")), toolBar);
    model->addBookmark(bookmark("YouTube", QUrl("http://www.youtube.com/")), toolBar);
    model->addBookmark(bookmark("Wikipedia", QUrl("http://www.wikipedia.org/")), toolBar);
}

void BookmarksToolBarContainer::storeVisibility(bool visible)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bookmarks"));
    settings.setValue(QLatin1String("toolbarVisible"), visible);
}

Q_EXPORT_PLUGIN(BookmarksPlugin)
