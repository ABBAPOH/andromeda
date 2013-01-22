#include "bookmarksplugin.h"
#include "bookmarkseditor.h"

#include <QtCore/QtPlugin>
#include <QtCore/QSettings>

#include <QtGui/QAction>
#include <QtGui/QFileIconProvider>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>

#include <ExtensionSystem/PluginManager>

#include <GuiSystem/AbstractDocument>
#include <GuiSystem/ActionManager>
#include <GuiSystem/EditorWindow>
#include <GuiSystem/EditorWindowFactory>
#include <GuiSystem/MenuBarContainer>
#include <GuiSystem/Command>
#include <GuiSystem/DocumentManager>
#include <GuiSystem/EditorManager>
#include <GuiSystem/ToolWidgetManager>
#include <GuiSystem/constants.h>

#include "bookmarksconstants.h"
#include "bookmarkdialog.h"
#include "bookmarksmenu.h"
#include "bookmarksdocument.h"
#include "bookmarksmodel.h"
#include "bookmarkstoolbar.h"
#include "bookmarkstoolwidget.h"
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

    BookmarksModel *model = BookmarksPlugin::instance()->sharedDocument()->model();

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
    button->setIcon(QIcon(":/bookmarks/icons/bookmarks.png"));
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
    MenuBarContainer *menuBarContainer = MenuBarContainer::instance();

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
    CommandContainer *viewMenu = MenuBarContainer::instance()->container(MenuBarContainer::ViewMenu);
    Command *c = new Command(Constants::Actions::ShowBookmarks, QKeySequence(), tr("Show Bookmarks toolbar"), this);
    c->setContext(Command::WindowCommand);
    viewMenu->addCommand(c);

    // ================ Bookmarks Menu ================
    BookmarksMenuContainer *menu = new BookmarksMenuContainer(Constants::Menus::Bookmarks, this);
    menu->setTitle(tr("Bookmarks"));
    menu->bookmarksMenu()->setInitialActions(actions);
    menu->bookmarksMenu()->setModel(m_model);
    connect(menu->bookmarksMenu(), SIGNAL(open(QUrl)), SLOT(open(QUrl)));
    connect(menu->bookmarksMenu(), SIGNAL(openInTabs(QList<QUrl>)), SLOT(openInTabs(QList<QUrl>)));
    connect(menu->bookmarksMenu(), SIGNAL(openInWindow(QList<QUrl>)), SLOT(openInWindow(QList<QUrl>)));
    menuBarContainer->addContainer(menu, "027");
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

void BookmarksToolBarContainer::storeVisibility(bool visible)
{
    QSettings settings;
    settings.beginGroup(QLatin1String("bookmarks"));
    settings.setValue(QLatin1String("toolbarVisible"), visible);
}

Q_EXPORT_PLUGIN(BookmarksPlugin)
