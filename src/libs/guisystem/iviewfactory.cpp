#include "iviewfactory.h"

#include "iview.h"

using namespace GuiSystem;

IViewFactory::IViewFactory(QObject* parent) :
    QObject(parent)
{
}

IViewFactory::~IViewFactory()
{
}

IView* IViewFactory::view()
{
    IView* result = createView();
    result->setFactory(this);

    return result;
}
