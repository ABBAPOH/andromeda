#ifndef BOOKMARKSWIDGET_P_H
#define BOOKMARKSWIDGET_P_H

#include "bookmarksmodel.h"
#include "bookmarkswidget.h"

#include <QtGui/QAction>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QToolBar>
#include <QtGui/QTreeView>
#include <QtGui/QVBoxLayout>

#include <QtGui/QSortFilterProxyModel>

namespace Bookmarks {

class FolderProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    FolderProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
        QAbstractItemModel *source = sourceModel();
        if (!source)
            return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

        BookmarksModel *model = static_cast<BookmarksModel*>(source);
        QModelIndex index = model->index(source_row, 0, source_parent);
        return model->isFolder(index);
    }

    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
    {
        if (source_column == 0)
            return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);

        return false;
    }
};

class BookmarkListFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    BookmarkListFilterModel(QObject *parent = 0) :
        QSortFilterProxyModel(parent)
    {}

    QModelIndex rootIndex() const { return m_rootIndex; }
    void setRootIndex(const QModelIndex &index) { m_rootIndex = index; invalidateFilter(); }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
    {
        QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
        QModelIndex pidx = m_rootIndex;
        while (pidx.isValid()) {
            if (pidx == idx)
                return true;
            pidx = pidx.parent();
        }

        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }

    QModelIndex m_rootIndex;
};

class BookmarksWidgetPrivate
{
public:
    QVBoxLayout *mainLayout;
    QToolBar *toolBar;
    QSplitter *splitter;

    // tool bar
    QAction *addFolderAction;
    QWidget *spacer;
    QLineEdit *lineEdit;

    // splitter
    QTreeView *treeView;
    QTreeView *tableView;

    BookmarksModel *model;
    FolderProxyModel *folderProxy;
    BookmarkListFilterModel *proxyModel;

    // actions
    QAction *openAction;
    QAction *openInTabAction;
    QAction *openInWindowAction;
    QAction *openInTabsAction;

    QAction *renameAction;
    QAction *editUrl;
    QAction *editDescription;

    QAction *removeAction;
};

} // namespace Bookmarks

#endif // BOOKMARKSWIDGET_P_H
