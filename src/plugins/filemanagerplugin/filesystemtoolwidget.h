#ifndef FILESYSTEMTOOLWIDGET_H
#define FILESYSTEMTOOLWIDGET_H

#include <GuiSystem/ToolWidget>
#include <GuiSystem/ToolWidgetFactory>

class QModelIndex;
class QUrl;

namespace FileManager {

class FileSystemToolWidget : public GuiSystem::ToolWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(FileSystemToolWidget)

public:
    explicit FileSystemToolWidget(QWidget *parent = 0);

private slots:
    void onActivated(const QModelIndex &index);
    void onUrlChanged(const QUrl &url);
    void open();
    void openInTab();
    void openInWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

protected:
    class TreeView;
    TreeView *m_view;
};

class FileSystemToolWidgetFactory : public GuiSystem::ToolWidgetFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileSystemToolWidgetFactory)

public:
    explicit FileSystemToolWidgetFactory(QObject *parent = 0) : ToolWidgetFactory("filesystem", parent) {}

    GuiSystem::ToolWidget *createToolWidget(QWidget *parent) const
    {
        return new FileSystemToolWidget(parent);
    }
};

} // namespace FileManager

#endif // FILESYSTEMTOOLWIDGET_H
