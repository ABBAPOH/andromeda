#ifndef FILEITEMDELEGATE_H
#define FILEITEMDELEGATE_H

#include "filemanager_global.h"

#include <QtGui/QAbstractItemDelegate>
#include <QtGui/QTextOption>
#include <QtGui/QTextLayout>

namespace FileManager {

class FileItemDelegatePrivate;
class FILEMANAGER_EXPORT FileItemDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(FileItemDelegate)
    Q_DISABLE_COPY(FileItemDelegate)
public:
    explicit FileItemDelegate(QObject *parent = 0);
    ~FileItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    bool eventFilter(QObject *object, QEvent *event);

protected:
    FileItemDelegatePrivate *d_ptr;
};

} // namespace FileManager

#endif // FILEITEMDELEGATE_H
