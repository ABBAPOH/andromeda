#ifndef FILEITEMDELEGATE_H
#define FILEITEMDELEGATE_H

#include <QtGui/QStyledItemDelegate>

namespace FileManager {

class FileItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    Q_DISABLE_COPY(FileItemDelegate)
public:
    explicit FileItemDelegate(QObject *parent = 0);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

} // namespace FileManager

#endif // FILEITEMDELEGATE_H
