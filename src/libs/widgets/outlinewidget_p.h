#ifndef OUTLINEWIDGET_P_H
#define OUTLINEWIDGET_P_H

#include <QtGui/QTreeView>
#include <QtGui/QStyledItemDelegate>

class QStandardItemModel;

class OutlineWidgetPrivate
{
public:
    QTreeView *treeView;
    QStandardItemModel *model;
    QList<QWidget *> widgets;
};

class OutlineTreeView : public QTreeView
{
public:
    explicit OutlineTreeView(QWidget *parent = 0);

    void drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const;

    QRect visualRect(const QModelIndex &index) const;
};

class OutlineDelegate : public QStyledItemDelegate
{
public:
    explicit OutlineDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // OUTLINEWIDGET_P_H
