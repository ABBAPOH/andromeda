#ifndef FILESYSTEMTOOLWIDGET_P_H
#define FILESYSTEMTOOLWIDGET_P_H

#include "filesystemtoolwidget.h"

#include <QtGui/QTreeView>

namespace FileManager {

class FileSystemToolWidget::TreeView : public QTreeView
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

} // namespace FileManager

#endif // FILESYSTEMTOOLWIDGET_P_H
