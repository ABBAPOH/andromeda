#ifndef HELLOWORLPLUGIN_H
#define HELLOWORLPLUGIN_H

#include <ExtensionSystem/IPlugin>
#include <GuiSystem/CommandContainer>

class QAction;
class QModelIndex;
class QUrl;

namespace Bookmarks {

class BookmarksModel;
class BookmarksDocument;

class BookmarksPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit BookmarksPlugin();

    static BookmarksPlugin *instance();

    bool initialize();
    void shutdown();

    BookmarksDocument *sharedDocument() const;

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
    Bookmarks::BookmarksModel *m_model;
    BookmarksDocument *m_document;

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
