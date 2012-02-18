#ifndef BOOKMARKSWIDGET_H
#define BOOKMARKSWIDGET_H

#include "bookmarks_global.h"

#include <QtGui/QWidget>

class QModelIndex;
class QUrl;

namespace Bookmarks {

class BookmarksModel;

class BookmarksWidgetPrivate;
class BOOKMARKS_EXPORT BookmarksWidget : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksWidget)

public:
    explicit BookmarksWidget(QWidget *parent = 0);
    ~BookmarksWidget();

    BookmarksModel *model() const;
    void setModel(BookmarksModel *model);

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    void open(const QList<QUrl> &urls);
    void open(const QUrl &url);
    void openInTab(const QUrl &url);
    void openInWindow(const QUrl &url);

    void stateChanged();

private slots:
    void onClicked(const QModelIndex &);
    void onDoubleClicked(const QModelIndex &);
    void onTextEdited(const QString &);
    void createFolder();
    void showTreeViewMenu(QPoint);
    void showTableViewMenu(QPoint);
    void openTriggered();
    void openInTabTriggered();
    void openInWindowTriggered();
    void openInTabsTriggered();
    void rename();
    void editUrl();
    void editDescription();
    void remove();

private:
    void setupUi();
    QModelIndex selectedIndex();
    QModelIndex selectedBookmarkIndex();

private:
    BookmarksWidgetPrivate *d;
};

} // namespace Bookmarks

#endif // BOOKMARKSWIDGET_H
