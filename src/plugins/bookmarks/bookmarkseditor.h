#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include "bookmarks_global.h"

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

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
    Bookmarks::BookmarksWidget *m_widget;
    Bookmarks::BookmarksModel *m_model;
    QSettings *m_settings;

    QAction *redoAction;
    QAction *undoAction;
};

class BookmarksEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit BookmarksEditorFactory(QObject *parent = 0);

    QByteArray id() const;
    QString name() const;
    QIcon icon() const;

protected:
    GuiSystem::AbstractDocument *createDocument(QObject *parent);
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace BookmarksPlugin

#endif // HELLOWORLDEDITOR_H
