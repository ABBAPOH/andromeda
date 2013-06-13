#ifndef BOOKMARKSTOOLWIDGET_P_H
#define BOOKMARKSTOOLWIDGET_P_H

#include "bookmarkstoolwidget.h"

#if QT_VERSION >= 0x050000
#include <QtWidgets/QTreeView>
#else
#include <QtGui/QTreeView>
#endif

namespace Bookmarks {

class BookmarksToolWidget::TreeView : public QTreeView
{
    Q_OBJECT
    Q_DISABLE_COPY(TreeView)

public:
    explicit TreeView(QWidget *parent = 0);

signals:
    void triggered(const QModelIndex &index);

protected:
    void keyPressEvent(QKeyEvent *event);
};

} // namespace Bookmarks

#endif // BOOKMARKSTOOLWIDGET_P_H
