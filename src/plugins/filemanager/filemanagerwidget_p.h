#ifndef FILEMANAGERWIDGET_P_H
#define FILEMANAGERWIDGET_P_H

#include "filemanagerwidget.h"

#include <QtCore/QMimeData>
#include <QtCore/QPointer>
#include <QtCore/QUrl>

#include <QtGui/QActionGroup>
#include <QtGui/QApplication>
#include <QtGui/QClipboard>
#include <QtGui/QColumnView>
#include <QtGui/QFileSystemModel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTableView>
#include <QtGui/QTreeView>
#include <QtGui/QUndoCommand>
#include <QtGui/QUndoStack>

#include "filesystemmanager.h"
#include "filesystemmodel.h"

namespace FileManager {

class FileDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit FileDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

public slots:
    void selectFileName();

private:
    mutable QPointer<QLineEdit> m_editor;
};

class FileManagerWidgetPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(FileManagerWidget)

public:
    FileManagerWidgetPrivate(FileManagerWidget *qq) : QObject(), q_ptr(qq) {}

    void setupUi();
    void createActions();
    void retranslateUi();
    void initViews();

    void setFileSystemManager(FileSystemManager *manager);

    void setModel(FileSystemModel *model);

    QModelIndexList selectedIndexes() const;
    void updateSorting();

    void registerAction(QAction *action, const QByteArray &id);

public slots:
    void onDoubleClick(const QModelIndex &index);
    void onCurrentItemIndexChanged(int index);
    void onSortIndicatorChanged(int logicalIndex, Qt::SortOrder order);

    void openNewTab();
    void openNewWindow();
    void toggleViewMode(bool);
    void toggleSortColumn(bool);
    void toggleSortOrder(bool descending);
    void onSelectionChanged();

public:
    FileManagerWidget::ViewMode viewMode;
    QAbstractItemView * currentView;
    QAbstractItemView * views[FileManagerWidget::MaxViews];
    QStackedLayout * layout;

    FileSystemModel *model;
    QString currentPath;

    FileSystemManager *fileSystemManager;

    FileManagerHistory * history;

    bool blockEvents;
    QSize gridSize;
    FileManagerWidget::Flow flow;

    bool itemsExpandable;

    FileManagerWidget::Column sortingColumn;
    Qt::SortOrder sortingOrder;

    QAction *actions[FileManagerWidget::ActionCount];

    QActionGroup *viewModeGroup;
    QActionGroup *sortByGroup;

private:
    FileManagerWidget *q_ptr;
};

} // namespace FileManager

#endif // FILEMANAGERWIDGET_P_H
