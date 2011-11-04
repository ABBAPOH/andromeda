#include "modeltoolbar.h"

#include "modelmenu.h"

#include <QtCore/QEvent>
#include <QtCore/QModelIndex>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolButton>

class ModelToolBarPrivate
{
public:
    ModelToolBarPrivate() :
        model(0)
    {}

public:
    QAbstractItemModel *model;
    QPersistentModelIndex rootIndex;
    QPoint dragStartPos;
};

Q_DECLARE_METATYPE(QModelIndex)

ModelToolBar::ModelToolBar(QWidget *parent) :
    QToolBar(parent),
    d(new ModelToolBarPrivate)
{
    if (isVisible())
        build();

    setAcceptDrops(true);
}

ModelToolBar::ModelToolBar(const QString &title, QWidget *parent) :
    QToolBar(title, parent),
    d(new ModelToolBarPrivate)
{
    if (isVisible())
        build();

    setAcceptDrops(true);
}

ModelToolBar::~ModelToolBar()
{
    delete d;
}

void ModelToolBar::setModel(QAbstractItemModel *model)
{
    if (d->model) {
        disconnect(d->model, SIGNAL(modelReset()),
                   this, SLOT(build()));
        disconnect(d->model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                   this, SLOT(build()));
        disconnect(d->model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                   this, SLOT(build()));
        disconnect(d->model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                   this, SLOT(build()));
    }

    d->model = model;

    if (d->model) {
        connect(d->model, SIGNAL(modelReset()),
                this, SLOT(build()));
        connect(d->model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
                this, SLOT(build()));
        connect(d->model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
                this, SLOT(build()));
        connect(d->model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
                this, SLOT(build()));
    }
}

QAbstractItemModel *ModelToolBar::model() const
{
    return d->model;
}

void ModelToolBar::setRootIndex(const QModelIndex &index)
{
    d->rootIndex = index;
}

QModelIndex ModelToolBar::rootIndex() const
{
    return d->rootIndex;
}

void ModelToolBar::build()
{
    Q_ASSERT(d->model);

    clear();

    prePopulated();

    for (int i = 0; i < d->model->rowCount(d->rootIndex); ++i) {
        QModelIndex index = d->model->index(i, 0, d->rootIndex);
        QVariant variant;
        variant.setValue(index);

        QString title = index.data(Qt::DisplayRole).toString();
        QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
        bool hasChildren = d->model->hasChildren(index);

        QAction *action = addAction(icon, title);
        action->setData(variant);

        QWidget *actionWidget = widgetForAction(action);
        QToolButton *button = qobject_cast<QToolButton*>(actionWidget);
        Q_ASSERT(button);
        button->installEventFilter(this);

        if (hasChildren) {
            ModelMenu *menu = createMenu();
            menu->setModel(d->model);
            menu->setRootIndex(index);
            action->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
            button->setArrowType(Qt::DownArrow);
        }
    }

    postPopulated();
}

QModelIndex ModelToolBar::index(QAction *action)
{
    if (!action)
        return QModelIndex();

    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return QModelIndex();

    QModelIndex index = qvariant_cast<QModelIndex>(variant);
    return index;
}

ModelMenu *ModelToolBar::createMenu()
{
    return new ModelMenu(this);
}

void ModelToolBar::prePopulated()
{
}

void ModelToolBar::postPopulated()
{
}

bool ModelToolBar::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QToolButton *button = static_cast<QToolButton*>(object);
        Q_ASSERT(button);

//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        QAction *action = button->defaultAction();
        Q_ASSERT(action);
        QModelIndex index = this->index(action);
        Q_ASSERT(this->index(action).isValid());
        emit activated(index);
    } else if (event->type() == QEvent::MouseButtonPress) {
        Q_ASSERT(static_cast<QToolButton*>(object));

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->buttons() & Qt::LeftButton)
            d->dragStartPos = mapFromGlobal(mouseEvent->globalPos());
    }

    return false;
}

void ModelToolBar::dragEnterEvent(QDragEnterEvent *event)
{
    if (!d->model) {
        QToolBar::dragEnterEvent(event);
        return;
    }

    QStringList mimeTypes = d->model->mimeTypes();
    foreach (const QString &mimeType, mimeTypes) {
        if (event->mimeData()->hasFormat(mimeType))
            event->acceptProposedAction();
    }

    QToolBar::dragEnterEvent(event);
}

void ModelToolBar::hideEvent(QHideEvent *event)
{
    clear();
    QToolBar::hideEvent(event);
}

void ModelToolBar::showEvent(QShowEvent *event)
{
    if (actions().isEmpty())
        build();
    QToolBar::showEvent(event);
}

void ModelToolBar::dropEvent(QDropEvent *event)
{
    if (!d->model) {
        QToolBar::dropEvent(event);
        return;
    }

    int row;
    QAction *action = actionAt(mapFromGlobal(QCursor::pos()));
    QModelIndex index;
    QModelIndex parentIndex = d->rootIndex;
    if (!action) {
        row = d->model->rowCount(d->rootIndex);
    } else {
        index = this->index(action);
        Q_ASSERT(index.isValid());
        row = index.row();

        if (d->model->hasChildren(index))
            parentIndex = index;
    }

    event->acceptProposedAction();
    d->model->dropMimeData(event->mimeData(), event->dropAction(), row, 0, parentIndex);
    QToolBar::dropEvent(event);
}

void ModelToolBar::mouseMoveEvent(QMouseEvent *event)
{
    if (!d->model) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    int manhattanLength = (event->pos() - d->dragStartPos).manhattanLength();
    if (manhattanLength <= QApplication::startDragDistance()) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    QAction *action = actionAt(d->dragStartPos);
    if (!action) {
        QToolBar::mouseMoveEvent(event);
        return;
    }

    QPersistentModelIndex index = this->index(action);
    Q_ASSERT(index.isValid());

    QDrag *drag = new QDrag(this);
    drag->setMimeData(d->model->mimeData(QModelIndexList() << index));
    QRect actionRect = actionGeometry(action);
    drag->setPixmap(QPixmap::grabWidget(this, actionRect));

    if (drag->exec() == Qt::MoveAction)
        d->model->removeRow(index.row(), d->rootIndex);
}
