#ifndef FILESYSTEMTOOLWIDGET_H
#define FILESYSTEMTOOLWIDGET_H

#include <Parts/ToolWidget>
#include <Parts/ToolWidgetFactory>

class QModelIndex;
class QUrl;

namespace FileManager {

class FileSystemToolWidget : public Parts::ToolWidget
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

class FileSystemToolWidgetFactory : public Parts::ToolWidgetFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(FileSystemToolWidgetFactory)

public:
    explicit FileSystemToolWidgetFactory(QObject *parent = 0) : ToolWidgetFactory("filesystem", parent) {}

    Parts::ToolWidget *createToolWidget(QWidget *parent) const
    {
        return new FileSystemToolWidget(parent);
    }
};

} // namespace FileManager

#endif // FILESYSTEMTOOLWIDGET_H
