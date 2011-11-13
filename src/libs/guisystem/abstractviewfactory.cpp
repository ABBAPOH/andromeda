#include "abstractviewfactory.h"

#include "abstractview.h"

using namespace GuiSystem;

/*!
    \brief Creates a AbstractViewFactory with the given \a parent.
*/
AbstractViewFactory::AbstractViewFactory(QObject *parent) :
    QObject(parent)
{
}

/*!
    \brief Destroys factory and all views created with it.
*/
AbstractViewFactory::~AbstractViewFactory()
{
    QList<AbstractView *> views = m_views;
    qDeleteAll(views);
}

AbstractView * AbstractViewFactory::view(QWidget *parent)
{
    AbstractView *view = createView(parent);
    view->setFactory(this);
    m_views.append(view);
    return view;
}
