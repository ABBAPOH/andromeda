#ifndef BOOKMARKSTOOLWIDGET_H
#define BOOKMARKSTOOLWIDGET_H

#include <GuiSystem/ToolWidget>
#include <GuiSystem/ToolWidgetFactory>

class QModelIndex;

namespace Bookmarks {

class BookmarksToolWidget : public GuiSystem::ToolWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksToolWidget)

public:
    explicit BookmarksToolWidget(QWidget *parent = 0);

private slots:
    void onActivated(const QModelIndex &index);

protected:
    class TreeView;
    TreeView *m_view;
};

class BookmarksToolWidgetFactory : public GuiSystem::ToolWidgetFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookmarksToolWidgetFactory)

public:
    explicit BookmarksToolWidgetFactory(QObject *parent = 0) : ToolWidgetFactory("bookmarks", parent) {}

    GuiSystem::ToolWidget *createToolWidget(QWidget *parent) const
    {
        return new BookmarksToolWidget(parent);
    }
};

} // namespace Bookmarks

#endif // BOOKMARKSTOOLWIDGET_H
