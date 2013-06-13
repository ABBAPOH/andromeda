#ifndef BOOKMARKSMENU_H
#define BOOKMARKSMENU_H

#include "bookmarks_global.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include <QtCore/QUrl>
#include <Widgets/ModelMenu>

namespace Bookmarks {

class BookmarksModel;

// Base class for BookmarksMenuBarMenu and BookmarksToolBarMenu
class BOOKMARKS_EXPORT BookmarksMenu : public ModelMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksMenu)

public:
    explicit BookmarksMenu(QWidget *parent = 0);

    BookmarksModel *model() const;
    void setModel(QAbstractItemModel *model);

signals:
    void open(const QUrl &url);
    void openInTabs(const QList<QUrl> &urls);
    void openInWindow(const QList<QUrl> &urls);

protected:
    void postPopulated();
    ModelMenu *createBaseMenu();

private slots:
    void openTabs();
    void openNewWindow();
    void activated(const QModelIndex &index);

private:
    QList<QUrl> getUrls() const;
};

// Menu that is dynamically populated from the bookmarks
class BOOKMARKS_EXPORT BookmarksMenuBarMenu : public BookmarksMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksMenuBarMenu)

public:
    explicit BookmarksMenuBarMenu(QWidget *parent = 0);

    void setInitialActions(QList<QAction*> actions);

protected:
    bool prePopulated();

private:
    QList<QAction*> m_initialActions;
};

} // namespace Bookmarks

#endif // BOOKMARKSMENU_H
