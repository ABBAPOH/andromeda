#include "outlinewidget.h"
#include "outlinewidget_p.h"

#include <QtCore/QModelIndex>
#include <QtGui/QHeaderView>
#include <QtGui/QPainter>
#include <QtGui/QResizeEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionFrameV3>

/*!
    \class OutlineTreeView

    Helper class for drawing line between rows.
*/
OutlineTreeView::OutlineTreeView(QWidget *parent) :
    QTreeView(parent)
{
}

void OutlineTreeView::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    QTreeView::drawRow(painter, options, index);

    if (index.parent().isValid())
        return;

    if (index.row() == 0)
        return;

    QStyleOptionFrameV3 opt;
    opt.frameShape = QFrame::HLine;
    opt.rect = QRect(0, options.rect.y(), options.rect.width(), 1);
    opt.version = 3;

    style()->drawPrimitive(QStyle::PE_Frame, &opt, painter);
}

void OutlineTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
    // This prevents drawing branch lines on win xp style
    QStyleOptionViewItemV2 option = viewOptions();

    if (isEnabled())
        option.state |= QStyle::State_Enabled;
    if (window()->isActiveWindow())
        option.state |= QStyle::State_Active;
    if (index.model()->hasChildren(index))
        option.state |= QStyle::State_Children;
    if (isExpanded(index))
        option.state |= QStyle::State_Open;
    option.rect = rect;

    style()->drawPrimitive(QStyle::PE_IndicatorBranch, &option, painter, this);
}

/*!
    \class OutlineWidget

    This class is a container for widgets that displayes them as two-level tree.
*/
OutlineWidget::OutlineWidget(QWidget *parent) :
    QFrame(parent),
    d_ptr(new OutlineWidgetPrivate)
{
    Q_D(OutlineWidget);

    d->treeView = new OutlineTreeView(this);
    d->treeView->setFocusPolicy(Qt::NoFocus);
    d->treeView->header()->hide();
    d->treeView->setExpandsOnDoubleClick(false);
    d->treeView->setFrameShape(QFrame::NoFrame);
    d->treeView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    d->treeView->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    QPalette pal = d->treeView->palette();
    pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    d->treeView->setPalette(pal);

    d->model = new QStandardItemModel(this);
    d->treeView->setModel(d->model);
}

OutlineWidget::~OutlineWidget()
{
    delete d_ptr;
}

QSize OutlineWidget::sizeHint() const
{
    Q_D(const OutlineWidget);

    int width = 0, height = 0;
    for (int i = 0; i < d->widgets.count(); i++) {
        QSize sizeHint = d->widgets[i]->sizeHint();
        int w = sizeHint.width();
        if (w > width)
            width = w;
        height += sizeHint.height() + d->treeView->sizeHintForRow(i) + 1;
    }
    return QSize(d->treeView->indentation() + width, height);
}

int OutlineWidget::addWidget(QWidget *widget, const QString &label)
{
    return addWidget(widget, QIcon(), label);
}

int OutlineWidget::addWidget(QWidget *widget, const QIcon &icon, const QString &label)
{
    Q_D(OutlineWidget);

    QStandardItem *root = new QStandardItem(icon, label);
    root->setFlags(Qt::ItemIsEnabled);
    QStandardItem *child = new QStandardItem;
    child->setFlags(Qt::ItemIsEnabled);
    root->appendRow(child);
    d->model->appendRow(root);
    QModelIndex index = child->index();
    d->treeView->setIndexWidget(index, widget);
    d->treeView->expand(index.parent());
    d->treeView->viewport()->installEventFilter(this);

    d->widgets.append(widget);
    return d->widgets.count() - 1;
}

bool OutlineWidget::isAnimated() const
{
    return d_func()->treeView->isAnimated();
}

void OutlineWidget::setAnimated(bool animated)
{
    d_func()->treeView->setAnimated(animated);
}

void OutlineWidget::clear()
{
    Q_D(OutlineWidget);

    d->model->clear();
    qDeleteAll(d->widgets);
    d->widgets.clear();
}

int OutlineWidget::count() const
{
    return d_func()->widgets.count();
}

QIcon OutlineWidget::icon(int index) const
{
    return d_func()->model->item(index)->icon();
}

void OutlineWidget::setIcon(int index, const QIcon &icon)
{
    d_func()->model->item(index)->setIcon(icon);
}

int OutlineWidget::insertWidget(int index, QWidget *widget, const QString &label)
{
    return insertWidget(index, widget, QIcon(), label);
}

int OutlineWidget::insertWidget(int index, QWidget *widget, const QIcon &icon, const QString &label)
{
    Q_D(OutlineWidget);

    if (index < 0)
        index = 0;
    if (index > count())
        index = count();

    QStandardItem *root = new QStandardItem(icon, label);
    root->setFlags(Qt::ItemIsEnabled);
    QStandardItem *child = new QStandardItem;
    child->setFlags(Qt::ItemIsEnabled);
    root->appendRow(child);
    d->model->insertRow(index, root);
    QModelIndex idx = child->index();
    d->treeView->setIndexWidget(idx, widget);
    d->treeView->expand(idx.parent());

    d->widgets.insert(index, widget);
    return index;
}

void OutlineWidget::remove(int index)
{
    Q_D(OutlineWidget);

    d->model->removeRow(index);
    d->widgets.takeAt(index)->deleteLater();
}

QString OutlineWidget::text(int index) const
{
    return d_func()->model->item(index)->text();
}

void OutlineWidget::setText(int index, const QString &label)
{
    d_func()->model->item(index)->setText(label);
}

int OutlineWidget::indexOf(QWidget *w) const
{
    return d_func()->widgets.indexOf(w);
}

QWidget * OutlineWidget::widget(int index) const
{
    Q_D(const OutlineWidget);

    if (index < 0 || index >= d->widgets.count())
        return 0;

    return d->widgets.at(index);
}

QSize OutlineWidget::minimumSizeHint() const
{
    Q_D(const OutlineWidget);

    int width = 0;
    for (int i = 0; i < d->widgets.count(); i++) {
        int w = d->widgets[i]->minimumSizeHint().width();
        if (w > width)
            width = w;
    }
    return QSize(width, d->treeView->minimumSizeHint().height());
}

void OutlineWidget::collapse(int index)
{
    Q_D(OutlineWidget);

    d->treeView->collapse(d->model->item(index)->index());
}

void OutlineWidget::collapseAll()
{
    d_func()->treeView->collapseAll();
}

void OutlineWidget::expand(int index)
{
    Q_D(OutlineWidget);

    d->treeView->expand(d->model->item(index)->index());
}

void OutlineWidget::expandAll()
{
    d_func()->treeView->expandAll();
}

bool OutlineWidget::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::ChildRemoved && o == d_func()->treeView->viewport()) {
        QChildEvent *ce = static_cast<QChildEvent *>(e);
        o = ce->child();

        if (o->isWidgetType()) {
            QWidget *w = static_cast<QWidget *>(o);
            int index = indexOf(w);
            if (index != -1) {
                d_func()->model->removeRow(index);
                d_func()->widgets.takeAt(index);
            }
        }
    }

    return QFrame::eventFilter(o, e);
}

void OutlineWidget::resizeEvent(QResizeEvent *e)
{
    QSize s = e->size();
    int width = lineWidth()/* + midLineWidth()*/;
    d_func()->treeView->move(width, width);
    d_func()->treeView->resize(QSize(s.width()-2*width, s.height()-2*width));
}
