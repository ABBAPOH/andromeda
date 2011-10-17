#ifndef FILEMANAGERWIDGET_P_H
#define FILEMANAGERWIDGET_P_H

#include "filemanagerwidget.h"

#include <QtCore/QMimeData>
#include <QtCore/QUrl>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QColumnView>
#include <QtGui/QFileSystemModel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>
#include <QtGui/QTableView>
#include <QtGui/QTreeView>
#include <QtGui/QUndoCommand>
#include <QtGui/QUndoStack>

#include "filesystemmanager.h"
#include "filesystemmodel.h"

namespace FileManagerPlugin {

class FileManagerWidgetPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FileManagerWidget)
    FileManagerWidget *q_ptr;

public:
    FileManagerWidgetPrivate(FileManagerWidget *qq) : QObject(), q_ptr(qq) {}

    void setupUi();
    void initViews();

    void setFileSystemManager(FileSystemManager *manager);

    QModelIndexList selectedIndexes() const;

public slots:
    void onDoubleClick(const QModelIndex &index);
    void onCurrentItemIndexChanged(int index);

public:
    FileManagerWidget::ViewMode viewMode;
    QAbstractItemView * currentView;
    QAbstractItemView * views[FileManagerWidget::MaxViews];
    QStackedLayout * layout;

    FileSystemModel *model;
    QString currentPath;

    FileSystemManager *fileSystemManager;

    CorePlugin::History * history;

    bool blockEvents;
    QSize gridSize;
    FileManagerWidget::Flow flow;
};

} // namespace FileManagerPlugin

#endif // FILEMANAGERWIDGET_P_H
