#include "modelmenu.h"

#include <QtCore/QAbstractItemModel>
#include <QtCore/QEvent>
#include <QtGui/QApplication>
#include <QtGui/QDragEnterEvent>

class ModelMenuPrivate
{
public:
    ModelMenuPrivate() :
        maxRows(-1),
        firstSeparator(-1),
        maxWidth(-1),
        statusBarTextRole(0),
        separatorRole(0),
        model(0)
    {}

public:
    int maxRows;
    int firstSeparator;
    int maxWidth;
    int statusBarTextRole;
    int separatorRole;

    QAbstractItemModel *model;
    QPersistentModelIndex root;
    QPoint dragStartPos;
};

Q_DECLARE_METATYPE(QModelIndex)

ModelMenu::ModelMenu(QWidget *parent) :
    QMenu(parent),
    d(new ModelMenuPrivate)
{
    setAcceptDrops(true);

    connect(this, SIGNAL(aboutToShow()), this, SLOT(aboutToShow()));
    connect(this, SIGNAL(triggered(QAction*)), this, SLOT(actionTriggered(QAction*)));
}

ModelMenu::~ModelMenu()
{
    delete d;
}

bool ModelMenu::prePopulated()
{
    return false;
}

void ModelMenu::postPopulated()
{
}

void ModelMenu::setModel(QAbstractItemModel *model)
{
    d->model = model;
}

QAbstractItemModel *ModelMenu::model() const
{
    return d->model;
}

void ModelMenu::setMaxRows(int max)
{
    d->maxRows = max;
}

int ModelMenu::maxRows() const
{
    return d->maxRows;
}

void ModelMenu::setFirstSeparator(int offset)
{
    d->firstSeparator = offset;
}

int ModelMenu::firstSeparator() const
{
    return d->firstSeparator;
}

void ModelMenu::setRootIndex(const QModelIndex &index)
{
    d->root = index;
}

QModelIndex ModelMenu::rootIndex() const
{
    return d->root;
}

void ModelMenu::setStatusBarTextRole(int role)
{
    d->statusBarTextRole = role;
}

int ModelMenu::statusBarTextRole() const
{
    return d->statusBarTextRole;
}

void ModelMenu::setSeparatorRole(int role)
{
    d->separatorRole = role;
}

int ModelMenu::separatorRole() const
{
    return d->separatorRole;
}

void ModelMenu::aboutToShow()
{
    clear();

    if (prePopulated())
        addSeparator();
    int max = d->maxRows;
    if (max != -1)
        max += d->firstSeparator;
    createMenu(d->root, max, this, this);
    postPopulated();
}

ModelMenu *ModelMenu::createBaseMenu()
{
    return new ModelMenu(this);
}

void ModelMenu::createMenu(const QModelIndex &parent, int max, QMenu *parentMenu, QMenu *menu)
{
    if (!menu) {
        QVariant v;
        v.setValue(parent);

        QString title = parent.data().toString();
        ModelMenu *modelMenu = createBaseMenu();
        // triggered goes all the way up the menu structure
        disconnect(modelMenu, SIGNAL(triggered(QAction*)),
                   modelMenu, SLOT(actionTriggered(QAction*)));
        modelMenu->setTitle(title);
        QIcon icon = qvariant_cast<QIcon>(parent.data(Qt::DecorationRole));
        modelMenu->setIcon(icon);
        parentMenu->addMenu(modelMenu)->setData(v);
        modelMenu->setRootIndex(parent);
        modelMenu->setModel(d->model);
        return;
    }

    if (!d->model)
        return;

    int end = d->model->rowCount(parent);
    if (max != -1)
        end = qMin(max, end);

    for (int i = 0; i < end; ++i) {
        QModelIndex idx = d->model->index(i, 0, parent);
        if (d->model->hasChildren(idx)) {
            createMenu(idx, -1, menu);
        } else {
            if (d->separatorRole != 0
                && idx.data(d->separatorRole).toBool())
                addSeparator();
            else
                menu->addAction(makeAction(idx));
        }
        if (menu == this && i == d->firstSeparator - 1)
            addSeparator();
    }
}

QAction *ModelMenu::makeAction(const QModelIndex &index)
{
    QIcon icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QString toolTip = index.data(Qt::ToolTipRole).toString();
    QAction *action = makeAction(icon, index.data().toString(), this);
    if (!toolTip.isEmpty())
        action->setToolTip(toolTip);
    action->setStatusTip(index.data(d->statusBarTextRole).toString());

    action->setCheckable(index.flags() & Qt::ItemIsUserCheckable);
    action->setChecked(index.data(Qt::CheckStateRole).toBool());

    QVariant v;
    v.setValue(index);
    action->setData(v);
    return action;
}

QAction *ModelMenu::makeAction(const QIcon &icon, const QString &text, QObject *parent)
{
    QFontMetrics fm(font());
    if (-1 == d->maxWidth)
        d->maxWidth = fm.width(QLatin1Char('m')) * 30;
    QString smallText = fm.elidedText(text, Qt::ElideMiddle, d->maxWidth);
    return new QAction(icon, smallText, parent);
}

void ModelMenu::actionTriggered(QAction *action)
{
    QModelIndex idx = index(action);

    if (d->model && action->isCheckable())
        d->model->setData(idx, action->isChecked() ? 2 : 0, Qt::CheckStateRole);

    if (idx.isValid())
        emit activated(idx);
}

QModelIndex ModelMenu::index(QAction *action)
{
    if (!action)
        return QModelIndex();
    QVariant variant = action->data();
    if (!variant.canConvert<QModelIndex>())
        return QModelIndex();

    return qvariant_cast<QModelIndex>(variant);
}

void ModelMenu::dragEnterEvent(QDragEnterEvent *event)
{
    if (!d->model) {
        QMenu::dragEnterEvent(event);
        return;
    }

    QStringList mimeTypes = d->model->mimeTypes();
    foreach (const QString &mimeType, mimeTypes) {
        if (event->mimeData()->hasFormat(mimeType))
            event->acceptProposedAction();
    }

    QMenu::dragEnterEvent(event);
}

void ModelMenu::dropEvent(QDropEvent *event)
{
    if (!d->model) {
        QMenu::dropEvent(event);
        return;
    }

    int row;
    QAction *action = actionAt(mapFromGlobal(QCursor::pos()));
    QModelIndex index;
    QModelIndex parentIndex = d->root;
    if (!action) {
        row = d->model->rowCount(d->root);
    } else {
        index = this->index(action);
        Q_ASSERT(index.isValid());
        row = index.row();

        if (d->model->hasChildren(index))
            parentIndex = index;
    }

    event->acceptProposedAction();
    d->model->dropMimeData(event->mimeData(), event->dropAction(), row, 0, parentIndex);
    QMenu::dropEvent(event);
}

void ModelMenu::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        d->dragStartPos = event->pos();
    QMenu::mousePressEvent(event);
}

void ModelMenu::mouseReleaseEvent(QMouseEvent *event)
{
    QMenu::mouseReleaseEvent(event);
}

void ModelMenu::mouseMoveEvent(QMouseEvent *event)
{
    int manhattanLength = (event->pos() - d->dragStartPos).manhattanLength();

    if (manhattanLength <= QApplication::startDragDistance()) {
        QMenu::mouseMoveEvent(event);
        return;
    }

    if (!(event->buttons() & Qt::LeftButton)) {
        QMenu::mouseMoveEvent(event);
        return;
    }

    QAction *action = actionAt(d->dragStartPos);
    QModelIndex idx = index(action);

    if (!idx.isValid()) {
        QMenu::mouseMoveEvent(event);
        return;
    }

    QDrag *drag = new QDrag(this);
    drag->setMimeData(d->model->mimeData((QModelIndexList() << idx)));
    QRect actionRect = actionGeometry(action);
    drag->setPixmap(QPixmap::grabWidget(this, actionRect));

    if (drag->exec() == Qt::MoveAction) {
        d->model->removeRow(idx.row(), d->root);

        if (!this->isAncestorOf(drag->target()))
            close();
        else
            aboutToShow();
    }
}
