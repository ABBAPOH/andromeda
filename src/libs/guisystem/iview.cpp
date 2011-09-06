#include "iview.h"

#include "iviewfactory.h"

#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

namespace GuiSystem {

class IViewPrivate
{
public:
    QPointer<QWidget> container;
    IViewFactory *factory;
    PerspectiveInstance *instance;
};

} // namespace GuiSystem

using namespace GuiSystem;

IView::IView(QObject *parent) :
    QObject(parent),
    d_ptr(new IViewPrivate)
{
}

IView::~IView()
{
    if (!container())
        qWarning() << "GuiSystem::IView::~IView - container has already been deleted";
    delete d_ptr->container;
    delete d_ptr;
}

QString IView::id() const
{
    return d_func()->factory->id();
}

QString IView::type() const
{
    return d_func()->factory->type();
}

QWidget * IView::container() const
{
    return d_func()->container;
}

void IView::setContainer(QWidget *widget)
{
    d_func()->container = widget;
}

PerspectiveInstance * IView::perspectiveInstance() const
{
    return d_func()->instance;
}

void IView::setPerspectiveInstance(PerspectiveInstance *instance)
{
    d_func()->instance = instance;
}

void IView::setFactory(IViewFactory *factory)
{
    d_func()->factory = factory;
}

ViewWidget::ViewWidget(QWidget *parent) :
    QWidget(parent)
{
    setObjectName("ViewWidget");

    m_view = 0;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    setLayout(layout);
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::setView(IView *view)
{
    m_view = view;
    QLayoutItem *child;
    while ( (child = layout()->takeAt(0)) != 0 ) {
        child->widget()->hide();
        delete child;
    }

    if (view->toolBar())
        layout()->addWidget(view->toolBar());
    layout()->addWidget(view->widget());
    if (view->toolBar())
        view->toolBar()->show();
    view->widget()->show();
}

QToolBar * ViewWidget::toolBar() const
{
    return m_view ? m_view->toolBar() : 0;
}

QWidget * ViewWidget::widget() const
{
    return m_view ? m_view->widget() : 0;
}
