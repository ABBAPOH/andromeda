#include "bookmarkstoolbar.h"

#include "bookmarksmenu.h"
#include "bookmarksmodel.h"

using namespace Bookmarks;

Q_DECLARE_METATYPE(QModelIndex)

BookmarksToolBar::BookmarksToolBar(QWidget *parent) :
    ModelToolBar(parent),
    m_bookmarksModel(0)
{
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenuRequested(const QPoint &)));
    connect(this, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(bookmarkActivated(const QModelIndex &)));
}

BookmarksModel * BookmarksToolBar::model() const
{
    return qobject_cast<BookmarksModel *>(ModelToolBar::model());
}

void BookmarksToolBar::setModel(QAbstractItemModel *m)
{
    BookmarksModel *model = qobject_cast<BookmarksModel*>(m);
    if (model) {
        m_bookmarksModel = model;
        ModelToolBar::setModel(model);
        setRootIndex(m_bookmarksModel->toolBar());
    } else {
        qWarning("Wrong model passed to BookmarksToolBar::setModel; expected BookmarksModel");
        m_bookmarksModel = 0;
        ModelToolBar::setModel(0);
    }
}

void BookmarksToolBar::setInitialActions(QList<QAction *> actions)
{
    m_initialActions = actions;
}

void BookmarksToolBar::contextMenuRequested(const QPoint &position)
{
    QAction *action = actionAt(position);

    QMenu menu;

    if (action) {
        QVariant variant = action->data();
        Q_ASSERT(variant.canConvert<QModelIndex>());

        QAction *menuAction = 0;

        if (!action->menu()) {
            menuAction = menu.addAction(tr("Open"), this, SLOT(openBookmarkInCurrentTab()));
            menuAction->setData(variant);

            menuAction = menu.addAction(tr("Open in New &Tab"), this, SLOT(openBookmarkInNewTab()));
            menuAction->setData(variant);

            menu.addSeparator();
        }

        menuAction = menu.addAction(tr("Remove"), this, SLOT(removeBookmark()));
        menuAction->setData(variant);

        menu.addSeparator();
    }

    menu.addAction(tr("Add Bookmark..."), this, SIGNAL(addBookmarkTriggered()));
    menu.addAction(tr("Add Folder..."), this, SIGNAL(addFolderTriggered()));

    menu.exec(QCursor::pos());
}

void BookmarksToolBar::bookmarkActivated(const QModelIndex &index)
{
    Q_ASSERT(index.isValid());

    emit open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksToolBar::openBookmark()
{
    QModelIndex index = ModelToolBar::index(qobject_cast<QAction*>(sender()));

    emit open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksToolBar::openBookmarkInCurrentTab()
{
    QModelIndex index = ModelToolBar::index(qobject_cast<QAction*>(sender()));

    emit open(index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksToolBar::openBookmarkInNewTab()
{
    QModelIndex index = ModelToolBar::index(qobject_cast<QAction*>(sender()));

    emit openInTabs(QList<QUrl>() << index.data(BookmarksModel::UrlRole).toUrl());
}

void BookmarksToolBar::removeBookmark()
{
    QModelIndex index = ModelToolBar::index(qobject_cast<QAction*>(sender()));

    if (m_bookmarksModel)
        m_bookmarksModel->removeRow(index.row(), rootIndex());
}

ModelMenu *BookmarksToolBar::createMenu()
{
    BookmarksMenu *menu = new BookmarksMenu(this);
    connect(menu, SIGNAL(open(const QUrl&)),
            this, SIGNAL(open(const QUrl&)));
    connect(menu, SIGNAL(openInTabs(QList<QUrl>)),
            this, SIGNAL(openInTabs(QList<QUrl>)));
    return menu;
}

void BookmarksToolBar::prePopulated()
{
    for (int i = 0; i < m_initialActions.count(); ++i)
        addAction(m_initialActions.at(i));
}
