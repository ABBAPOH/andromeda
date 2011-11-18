#ifndef HELLOWORLDEDITOR_H
#define HELLOWORLDEDITOR_H

#include <guisystem/abstracteditor.h>
#include <guisystem/abstracteditorfactory.h>

namespace Bookmarks {
class BookmarksModel;
class BookmarksWidget;
}

namespace CorePlugin {
class Settings;
}

namespace BookmarksPlugin {

class BookmarksEditor : public GuiSystem::AbstractEditor
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksEditor)

public:
    explicit BookmarksEditor(QWidget *parent = 0);

    void open(const QUrl &url);
    QUrl url() const;

    QIcon icon() const;
    QString title() const;
    QString windowTitle() const;

    QByteArray saveState() const;
    void restoreState(const QByteArray &state);

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
    CorePlugin::Settings *m_settings;

    QAction *redoAction;
    QAction *undoAction;
};

class BookmarksEditorFactory : public GuiSystem::AbstractEditorFactory
{
public:
    explicit BookmarksEditorFactory(QObject *parent = 0);

    QStringList mimeTypes();
    QByteArray id() const;

protected:
    GuiSystem::AbstractEditor *createEditor(QWidget *parent);
};

} // namespace BookmarksPlugin

#endif // HELLOWORLDEDITOR_H
