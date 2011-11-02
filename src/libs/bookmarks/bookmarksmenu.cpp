#include "bookmarksmenu.h"

#include "bookmarksmodel.h"

using namespace Bookmarks;

BookmarksMenu::BookmarksMenu(QWidget *parent) :
    ModelMenu(parent)
{
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(activated(const QModelIndex &)));
    setStatusBarTextRole(BookmarksModel::UrlRole);
}

BookmarksModel * BookmarksMenu::model() const
{
    return qobject_cast<BookmarksModel *>(ModelMenu::model());
}

void BookmarksMenu::setModel(QAbstractItemModel *model)
{
    if (!qobject_cast<BookmarksModel*>(model))
        qWarning("Wrong model passed to BookmarksMenu::setModel; expected BookmarksModel");
    ModelMenu::setModel(model);
}

ModelMenu *BookmarksMenu::createBaseMenu()
{
    BookmarksMenu *menu = new BookmarksMenu(this);
    connect(menu, SIGNAL(open(const QUrl&)),
            this, SIGNAL(open(const QUrl&)));
    connect(menu, SIGNAL(openInTabs(const QList<QUrl>&)),
            this, SIGNAL(openInTabs(const QList<QUrl>&)));
    connect(menu, SIGNAL(openInWindow(const QList<QUrl>&)),
            this, SIGNAL(openInWindow(const QList<QUrl>&)));
    return menu;
}

void BookmarksMenu::activated(const QModelIndex &index)
{
    emit open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksMenu::postPopulated()
{
    if (isEmpty())
        return;

    QModelIndex parent = rootIndex();

    bool hasBookmarks = false;

    BookmarksModel *model = this->model();
    if (!model)
        return;

    for (int i = 0; i < model->rowCount(parent); ++i) {
        QModelIndex child = model->index(i, 0, parent);

        if (!model->isFolder(child)) {
            hasBookmarks = true;
            break;
        }
    }

    if (!hasBookmarks)
        return;

    addSeparator();
    QAction *action = addAction(tr("Open in Tabs"));
    connect(action, SIGNAL(triggered()),
            this, SLOT(openTabs()));

    action = addAction(tr("Open in New Window"));
    connect(action, SIGNAL(triggered()),
            this, SLOT(openNewWindow()));
}

void BookmarksMenu::openTabs()
{
    QList<QUrl> urls = getUrls();
    if (!urls.isEmpty())
        emit openInTabs(urls);
}

void BookmarksMenu::openNewWindow()
{
    QList<QUrl> urls = getUrls();
    if (!urls.isEmpty())
        emit openInWindow(urls);
}

BookmarksMenuBarMenu::BookmarksMenuBarMenu(QWidget *parent) :
    BookmarksMenu(parent)
{
}

bool BookmarksMenuBarMenu::prePopulated()
{
    BookmarksModel *model = this->model();
    if (!model)
        return false;

    setRootIndex(model->menu());
    // initial actions
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
    if (!m_initialActions.isEmpty())
        addSeparator();
    createMenu(model->toolBar(), 1, this);

    return true;
}

void BookmarksMenuBarMenu::setInitialActions(QList<QAction*> actions)
{
    m_initialActions = actions;
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
}

QList<QUrl> BookmarksMenu::getUrls() const
{
    ModelMenu *menu = qobject_cast<ModelMenu*>(sender()->parent());
    if (!menu)
        return QList<QUrl>();

    QModelIndex parent = menu->rootIndex();
    if (!parent.isValid())
        return QList<QUrl>();

    BookmarksModel *model = this->model();
    if (!model)
        return QList<QUrl>();

    QList<QUrl> urls;
    for (int i = 0; i < model->rowCount(parent); ++i) {
        QModelIndex child = model->index(i, 0, parent);

        if (model->isFolder(child))
            continue;

        urls.append(child.data(BookmarksModel::UrlRole).toUrl());
    }
    return urls;
}
