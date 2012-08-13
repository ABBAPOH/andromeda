#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <extensionsystem/iplugin.h>
#include <guisystem/commandcontainer.h>

class QAction;
class QModelIndex;
class QUrl;

namespace Bookmarks {

class BookmarksModel;

class BookmarksPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit BookmarksPlugin();

    bool initialize();
    void shutdown();

private slots:
    void open(const QUrl &url);
    void openInTabs(const QList<QUrl> &urls);
    void openInWindow(const QList<QUrl> &urls);

    void showBookmarks();
    void addBookmark();
    void addFolder();

private:
    void createActions();
    void showBookmarkDialog(const QModelIndex &index, bool isFolder);
    void addDefaultBookmarks();

private:
    Bookmarks::BookmarksModel *model;

    QAction *addBookmarkAction;
    QAction *addFolderAction;
    QAction *showBookmarksAction;
};

class BookmarksToolBarContainer : public GuiSystem::CommandContainer
{
    Q_OBJECT

public:
    explicit BookmarksToolBarContainer(const QByteArray &id, QObject *parent = 0) :
        CommandContainer(id, parent)
    {}
    ~BookmarksToolBarContainer();

    QToolBar *createToolBar(QWidget *parent) const;

signals:
    void open(const QUrl &url);
    void openInTabs(const QList<QUrl> &urls);

    void showBookmarksTriggered();
    void addBookmarkTriggered();
    void addFolderTriggered();

private slots:
    void storeVisibility(bool visible);
    void onDestroy(QObject *);

private:
    QList<QObject *> toolBars;
};

} // namespace Bookmarks

#endif // HELLOWORLPLUGIN_H
