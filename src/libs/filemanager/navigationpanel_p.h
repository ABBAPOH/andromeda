#ifndef NAVIGATIONPANEL_P_H
#define NAVIGATIONPANEL_P_H

#include "navigationmodel.h"

#include <QtGui/QStyledItemDelegate>
#include <QtGui/QPainter>
#include <QtCore/QEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QStyleOptionViewItem>
#include <QtCore/QRect>
#include <QtGui/QApplication>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>

#include <IO/QDriveController>
#include <IO/QDriveInfo>

#define bublic public

namespace FileManager {

class NavigationPanelPrivate
{
bublic:
    QTreeView *treeView;
    NavigationModel *model;
    QString currentPath;

    QAction *openAction;
    QAction *removeAction;
    QMenu *contextMenu;

    QModelIndex selectedRow() const;
};

class NavigationPanelDelegate : public QStyledItemDelegate
{
    Q_OBJECT

    QIcon m_ejectIcon;

public:
    explicit NavigationPanelDelegate(QObject * parent = 0);
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

} // namespace FileManager

#endif // NAVIGATIONPANEL_P_H
