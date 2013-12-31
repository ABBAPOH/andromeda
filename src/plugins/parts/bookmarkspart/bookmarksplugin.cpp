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
#include <Parts/ContextCommand>
#include <Parts/DocumentManager>
#include <Parts/EditorManager>
#include <Parts/ModelContainer>
#include <Parts/OpenStrategy>
#include <Parts/Separator>
#include <Parts/ToolWidgetManager>
#include <Parts/constants.h>

#include <Bookmarks/BookmarksDialog>
#include <Bookmarks/BookmarksModel>

#include "bookmarksconstants.h"
#include "bookmarksdocument.h"
#include "bookmarkseditor.h"
#include "bookmarksmenucontainer.h"
#include "bookmarkstoolwidget.h"

using namespace ExtensionSystem;
using namespace Parts;
using namespace Bookmarks;

BookmarksButtonCommand::BookmarksButtonCommand(QObject *parent) :
    AbstractCommand("BookmarksButton", parent)
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
    OpenStrategy *strategy = OpenStrategy::defaultStrategy();
    if (!strategy)
        return;
    strategy->open(url);
}

void BookmarksPlugin::openInTabs(const QList<QUrl> &urls)
{
    // TODO: remove?
    OpenStrategy *strategy = OpenStrategy::strategy(OpenStrategy::TypeOpenNewTab);
    if (!strategy)
        return;
    strategy->open(urls);
}

void BookmarksPlugin::openInWindow(const QList<QUrl> &urls)
{
    OpenStrategy *strategy = OpenStrategy::strategy(OpenStrategy::TypeOpenNewWindow);
    if (!strategy)
        return;
    strategy->open(urls);
}

void BookmarksPlugin::onIndexTriggered(const QModelIndex &index)
{
    open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksPlugin::showBookmarks()
{
    OpenStrategy *strategy = OpenStrategy::defaultStrategy();
    if (!strategy)
        return;
    strategy->open(AbstractEditor::editorUrl(Constants::Editors::Bookmarks));
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
    BookmarksModel *model = this->model();

    ModelContainer * toolBar = new ModelContainer("AlternateToolbar", this);
    toolBar->setModel(model, model->toolBar());
    toolBar->addCommand(new BookmarksButtonCommand(toolBar), toolBar->firstSeparator());
    connect(toolBar, SIGNAL(triggered(QModelIndex)), this, SLOT(onIndexTriggered(QModelIndex)));

    // ================ Bookmarks Menu ================
    BookmarksMenuContainer *menu = new BookmarksMenuContainer(Constants::Menus::Bookmarks, this);
    menu->setText(tr("Bookmarks"));
    menu->setModel(model, model->menu());
    connect(menu, SIGNAL(triggered(QModelIndex)), this, SLOT(onIndexTriggered(QModelIndex)));
    addObject(menu);

    ApplicationCommand *addBookmark = new ApplicationCommand("AddBookmark", this);
    addBookmark->setText(tr("Add bookmark"));
    addBookmark->setShortcut(QKeySequence("Ctrl+D"));
    connect(addBookmark, SIGNAL(triggered()), SLOT(addBookmark()));
    menu->addCommand(addBookmark, menu->firstSeparator());

    ApplicationCommand *addFolder = new ApplicationCommand("AddFolder", this);
    addFolder->setText(tr("Add folder"));
    addFolder->setShortcut(QKeySequence("Ctrl+Alt+F"));
    connect(addFolder, SIGNAL(triggered()), SLOT(addFolder()));
    menu->addCommand(addFolder, menu->firstSeparator());

    ApplicationCommand *showBookmarks = new ApplicationCommand("ShowBookmarks", this);
    showBookmarks->setText(tr("Show bookmarks"));
    showBookmarks->setShortcut(QKeySequence("Ctrl+Alt+B"));
    connect(showBookmarks, SIGNAL(triggered()), SLOT(showBookmarks()));
    menu->addCommand(showBookmarks, menu->firstSeparator());

    // ================ View Menu ================
    ContextCommand *c = new ContextCommand(Constants::Actions::ShowBookmarksToolbar, this);
    c->setShortcut(QKeySequence());
    c->setText(tr("Show Bookmarks toolbar"));
    c->setAttributes(ContextCommand::AttributeUpdateEnabled);
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
