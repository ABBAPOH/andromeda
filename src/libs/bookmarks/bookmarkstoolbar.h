#ifndef BOOKMARKSTOOLBAR_H
#define BOOKMARKSTOOLBAR_H

#include "bookmarks_global.h"

#include <QtCore/QUrl>
#include <modeltoolbar.h>

class QAbstractItemModel;

namespace Bookmarks {

class BookmarksModel;
class BOOKMARKS_EXPORT BookmarksToolBar : public ModelToolBar
{
    Q_OBJECT

public:
    explicit BookmarksToolBar(QWidget *parent = 0);

    BookmarksModel *model() const;
    void setModel(QAbstractItemModel *model);

    void setInitialActions(QList<QAction*> actions);

signals:
    void open(const QUrl &url);
    void openInTabs(const QList<QUrl> &urls);

    void addBookmarkTriggered();
    void addFolderTriggered();

protected:
    ModelMenu *createMenu();
    void prePopulated();

private slots:
    void contextMenuRequested(const QPoint &position);

protected slots:
    void openBookmark();
    void openBookmarkInCurrentTab();
    void openBookmarkInNewTab();
    void removeBookmark();
    void bookmarkActivated(const QModelIndex &);

private:
    BookmarksModel *m_bookmarksModel;

    QPoint m_dragStartPosition;
    QList<QAction*> m_initialActions;
};

} // namespace Bookmarks

#endif // BOOKMARKSTOOLBAR_H
