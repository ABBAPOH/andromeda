#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include "bookmarks_global.h"

#include <GuiSystem/AbstractEditor>
#include <GuiSystem/AbstractEditorFactory>

class QSettings;

namespace Bookmarks {
class BookmarksModel;
class BookmarksWidget;
}

namespace Bookmarks {

class BookmarksDocument;

class BOOKMARKS_EXPORT BookmarksEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksEditor)

public:
    explicit BookmarksEditor(QWidget *parent = 0);
    explicit BookmarksEditor(BookmarksDocument &document, QWidget *parent = 0);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

private slots:
    void openTriggered(const QUrl &url);
    void openInTabTriggered(const QUrl &url);
    void openInWindowTriggered(const QUrl &url);

    void onStateChanged();

protected:
    void resizeEvent(QResizeEvent *);

private:
    void init();

private:
    Bookmarks::BookmarksWidget *m_widget;
    QSettings *m_settings;

    QAction *redoAction;
    QAction *undoAction;
};

class BookmarksEditorFactory : public GuiSystem::AbstractEditorFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksEditorFactory)

public:
    explicit BookmarksEditorFactory(QObject *parent = 0);

    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace BookmarksPlugin

#endif // HELLOWORLDEDITOR_H
