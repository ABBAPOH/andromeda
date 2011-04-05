#include "iviewfactory.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>

#include "iview.h"
#include "state.h"

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
    result->setFactoryId(id());
//    if (result->parent() && result->parent() != this) {
//        qWarning() << "object of type" << result->metaObject()->className() << "already has parent, fixing";
//    }
//    result->setParent(this);
    return result;
}
