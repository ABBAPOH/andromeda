#include "iviewfactory.h"

#include <QtCore/QMap>

#include "iview.h"
#include "state.h"

namespace GuiSystem {

class IViewFactoryPrivate
{
public:
    QMap<QObject*, IView*> sharedViews;
};

} // namespace GuiSystem

using namespace GuiSystem;

IViewFactory::IViewFactory(QObject * parent) :
    QObject(parent),
    d_ptr(new IViewFactoryPrivate)
{
}

IViewFactory::~IViewFactory()
{
    delete d_ptr;
}

IView *IViewFactory::view(State *state)
{
    Q_ASSERT_X(state, "IViewFactory::view", "State can't be 0");

    if (shareMode() == Clone) {

        IView *view = createView(state);
        view->setParent(state); // should reparent to correctly delete object
        return view;

    } else {

        Q_D(IViewFactory);

        QObject *parent = 0;
        if (shareMode() == ShareState)
            parent = state;
        else if (shareMode() == ShareWindow)
            parent = state->parent(); // TODO: state->session() ??

        IView *view = d->sharedViews.value(parent);
        if (!view) {
            view = createView(parent);
            view->setParent(parent); // should reparent to correctly delete object
            d->sharedViews.insert(parent, view);
        }
        return view;

    }
}
