#include "navigationpanel.h"
#include "navigationpanel_p.h"

#include "navigationmodel.h"

#include <io/QDriveController>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <QtGui/QResizeEvent>
#include <QtGui/QHeaderView>

using namespace FileManager;

QModelIndex NavigationPanelPrivate::selectedRow() const
{
    QModelIndexList indexes = treeView->selectionModel()->selectedRows();
    if (indexes.isEmpty())
        return QModelIndex();

    return indexes.first();
}

NavigationPanelDelegate::NavigationPanelDelegate(QObject* parent):
    QStyledItemDelegate(parent)
{
    m_ejectIcon = QIcon(":/icons/eject.png");
}

bool NavigationPanelDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        QRect rect = option.rect;
        rect.setX(rect.x() + rect.width() - (rect.height()+BORDER)); // draw icon (size is rect's height*height)
        rect.setRight(rect.right()-BORDER);
        if ( rect.contains(me->x(), me->y()) ) {
            const NavigationModel *model = qobject_cast<const NavigationModel*>(index.model());
            if (model) {
                QDriveInfo driveInfo = model->driveInfo(index);
                if ( driveInfo.isValid() && ( driveInfo.type() == QDriveInfo::RemoteDrive || driveInfo.type() == QDriveInfo::RemovableDrive ))
                    QDriveController().eject(model->path(index));
            }

            return true;
        }
    }

    return false;
}

void NavigationPanelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool paint = true;

//#if defined(Q_OS_WIN)
    paint = false;
    QStyledItemDelegate::paint(painter, option, index);
//#endif

    const NavigationModel *model = qobject_cast<const NavigationModel*>(index.model());
    if (model) {
        QDriveInfo drive = model->driveInfo(index);

        if ( drive.isValid() && ( drive.type() == QDriveInfo::RemoteDrive || drive.type() == QDriveInfo::RemovableDrive ) ){
            QStyleOptionViewItemV4 optionRight = option;
            optionRight.viewItemPosition = QStyleOptionViewItemV4::End;
            optionRight.rect.setX(optionRight.rect.x() + optionRight.rect.width() - (optionRight.rect.height() + BORDER)); // draw icon (size is rect's height*height)

//#if !defined(Q_OS_WIN)
            QStyleOptionViewItemV4 optionLeft = option;
            // decrease text width so icon won't overlap text
            optionLeft.rect.setWidth(optionLeft.rect.width() - (optionLeft.rect.height() + BORDER));
            optionLeft.viewItemPosition = QStyleOptionViewItemV4::Beginning;
            QStyledItemDelegate::paint(painter, optionLeft, index);

            QStyle *style = QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &optionRight, painter);
//#endif
            QRect rect = optionRight.rect;
            rect.setRight(rect.right()-BORDER);
            m_ejectIcon.paint(painter, rect);

            return;
        }
    }

    if( paint )
        QStyledItemDelegate::paint(painter, option, index);
}

NavigationPanel::NavigationPanel(QWidget *parent) :
    QWidget(parent),
    d_ptr(new NavigationPanelPrivate)
{
    Q_D(NavigationPanel);

    d->model = new NavigationModel(this);
    d->treeView = new QTreeView(this);

    d->treeView->setModel(d->model);
    d->treeView->setHeaderHidden(true);
    d->treeView->setFocusPolicy(Qt::NoFocus);
    d->treeView->setAcceptDrops(true);
    d->treeView->setDropIndicatorShown(true);
    d->treeView->setDragEnabled(true);
    d->treeView->setDragDropMode(QAbstractItemView::DragDrop);

    QPalette pal = d->treeView->palette();
    pal.setColor(QPalette::Base, QColor(214, 221, 229));
    d->treeView->QAbstractItemView::setPalette(pal);
    d->treeView->expandAll();

    connect(d->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));

    d->contextMenu = new QMenu(this);
    d->openAction = d->contextMenu->addAction(tr("Open"), this, SLOT(onOpenTriggered()));
    d->contextMenu->addSeparator();
    d->removeAction = d->contextMenu->addAction(tr("Remove from panel"), this, SLOT(onRemoveTriggered()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);

    QAbstractItemDelegate *delegate = d->treeView->itemDelegate();
    if (delegate)
        delegate->deleteLater();
    d->treeView->setItemDelegate(new NavigationPanelDelegate());

    setMinimumSize(100, 200);
}

NavigationPanel::~NavigationPanel()
{
    delete d_ptr;
}

void NavigationPanel::addFolder(const QString & path)
{
    Q_D(NavigationPanel);

    d->model->addFolder(path);
}

void NavigationPanel::removeFolder(const QString &path)
{
    Q_D(NavigationPanel);

    d->model->removeFolder(path);
}

QString NavigationPanel::currentPath() const
{
    return d_func()->currentPath;
}

NavigationModel * NavigationPanel::model() const
{
    return d_func()->model;
}

void NavigationPanel::setModel(NavigationModel *model)
{
    Q_D(NavigationPanel);

    if (d->model && d->model->QObject::parent() == this)
        d->model->deleteLater();

    if (d->model != model) {
        d->model = model;
        d->treeView->setModel(model);
        d->treeView->expandAll();
    }
}

void NavigationPanel::resizeEvent(QResizeEvent * event)
{
    Q_D(NavigationPanel);

    d->treeView->resize(event->size());
    QWidget::resizeEvent(event);
}

void NavigationPanel::onClick(const QModelIndex &index)
{
    Q_D(NavigationPanel);

    QString path = d->model->path(index);

    if (!path.isEmpty()) {
        d->currentPath  = path;
        emit currentPathChanged(path);
        emit triggered(path);
    } else {
        d->treeView->selectionModel()->select(d->model->index(d->currentPath), QItemSelectionModel::Select);
    }
}

void NavigationPanel::onCustomContextMenuRequested(QPoint p)
{
    Q_D(NavigationPanel);

    QModelIndex index = d->selectedRow();
    if (!index.isValid())
        return;

    // fixme : awful code
    d->removeAction->setEnabled(index.parent().row()==2);
    d->contextMenu->exec(mapToGlobal(p));
}

void NavigationPanel::onOpenTriggered()
{
    Q_D(NavigationPanel);

    emit triggered(d->model->path(d->selectedRow()));
}

void NavigationPanel::onRemoveTriggered()
{
    Q_D(NavigationPanel);

    d->model->removeFolder(d->model->path(d->selectedRow()));
}



