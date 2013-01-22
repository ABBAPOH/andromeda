#include "navigationpanel.h"
#include "navigationpanel_p.h"

#include "navigationmodel.h"

#include <IO/QDriveController>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <QtGui/QResizeEvent>
#include <QtGui/QHeaderView>

static const int BORDER = 4;

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
    m_ejectIcon = QIcon(":/filemanager/icons/eject.png");
}

bool NavigationPanelDelegate::editorEvent(QEvent *event,
                                          QAbstractItemModel *model,
                                          const QStyleOptionViewItem &option,
                                          const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        QRect rect = option.rect;
        rect.setX(rect.x() + rect.width() - (rect.height() + BORDER)); // draw icon (size is rect's height*height)
        rect.setRight(rect.right() - BORDER);
        if (rect.contains(me->x(), me->y())) {
            const NavigationModel *navigationModel = qobject_cast<const NavigationModel*>(model);
            if (navigationModel) {
                QDriveInfo driveInfo = navigationModel->driveInfo(index);
                if (driveInfo.isValid() &&
                        (driveInfo.type() == QDriveInfo::RemoteDrive ||
                         driveInfo.type() == QDriveInfo::RemovableDrive ||
                         driveInfo.type() == QDriveInfo::CdromDrive))
                    QDriveController().eject(navigationModel->path(index));
            }

            return true;
        }
    }

    return false;
}

void NavigationPanelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool paint = true;

    paint = false;
    QStyledItemDelegate::paint(painter, option, index);

#if !defined(Q_OS_WIN)
    const NavigationModel *model = qobject_cast<const NavigationModel*>(index.model());
    if (model) {
        QDriveInfo drive = model->driveInfo(index);

        if (drive.isValid() &&
                (drive.type() == QDriveInfo::RemoteDrive ||
                 drive.type() == QDriveInfo::RemovableDrive ||
                 drive.type() == QDriveInfo::CdromDrive)) {
            QStyleOptionViewItemV4 optionRight = option;
            optionRight.viewItemPosition = QStyleOptionViewItemV4::End;
            optionRight.rect.setX(optionRight.rect.x() +
                                  optionRight.rect.width() -
                                  (optionRight.rect.height() + BORDER)); // draw icon (size is rect's height*height)

            QStyleOptionViewItemV4 optionLeft = option;
            // decrease text width so icon won't overlap text
            optionLeft.rect.setWidth(optionLeft.rect.width() - (optionLeft.rect.height() + BORDER));
            optionLeft.viewItemPosition = QStyleOptionViewItemV4::Beginning;
            QStyledItemDelegate::paint(painter, optionLeft, index);

            QStyle *style = QApplication::style();
            style->drawControl(QStyle::CE_ItemViewItem, &optionRight, painter);
            QRect rect = optionRight.rect;
            rect.setRight(rect.right() - BORDER);
            m_ejectIcon.paint(painter, rect);

            return;
        }
    }

    if (paint)
        QStyledItemDelegate::paint(painter, option, index);
#endif
}

/*!
    \class FileManager::NavigationPanel

    \brief NavigationPanel is a widget for displaying NavigationModel.

    This widget simply displays NavigationModel and provides some convenience
    methods.

    \image html navigationpanel.png
*/

/*!
    Creates NavigationPanel with the given \a parent.
*/
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
    d->treeView->setRootIsDecorated(false);
    d->treeView->setItemsExpandable(false);
    d->treeView->setIndentation(0);
    d->treeView->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    QPalette pal = d->treeView->palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    d->treeView->setPalette(pal);
    d->treeView->expandAll();

    connect(d->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onClick(QModelIndex)));

    d->contextMenu = new QMenu(this);
    d->openAction = d->contextMenu->addAction(tr("Open"), this, SLOT(onOpenTriggered()));
    d->contextMenu->addSeparator();
    d->removeAction = d->contextMenu->addAction(tr("Remove from panel"), this, SLOT(onRemoveTriggered()));

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(onCustomContextMenuRequested(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);

    d->treeView->setItemDelegate(new NavigationPanelDelegate(this));

    setMinimumSize(100, 200);
}

/*!
    Destroys NavigationPanel.
*/
NavigationPanel::~NavigationPanel()
{
    delete d_ptr;
}

/*!
    Adds new \a path to the model. Path is appended to the end of list.
*/
void NavigationPanel::addFolder(const QString & path)
{
    Q_D(NavigationPanel);

    if (d->model)
        d->model->addFolder(path);
}

/*!
    Removes new \a path from the model.
*/
void NavigationPanel::removeFolder(const QString &path)
{
    Q_D(NavigationPanel);

    if (d->model)
        d->model->removeFolder(path);
}

/*!
    \property NavigationPanel::currentPath

    This property holds current selected path.
*/

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

/*!
    \reimp
*/
void NavigationPanel::resizeEvent(QResizeEvent * event)
{
    Q_D(NavigationPanel);

    d->treeView->resize(event->size());
    QWidget::resizeEvent(event);
}

/*!
    \fn void NavigationPanel::currentPathChanged(const QString & path)

    This signal is emitted when NavigationPanel::currentPath property is changed.
*/

/*!
    \fn void triggered(const QString & path)

    This signal is emitted when user requests to open file or folder by
    clicking on it or toggling action in a popup menu.
*/

/*!
    \internal
*/
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

/*!
    \internal
*/
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

/*!
    \internal
*/
void NavigationPanel::onOpenTriggered()
{
    Q_D(NavigationPanel);

    emit triggered(d->model->path(d->selectedRow()));
}

/*!
    \internal
*/
void NavigationPanel::onRemoveTriggered()
{
    Q_D(NavigationPanel);

    d->model->removeFolder(d->model->path(d->selectedRow()));
}
