#include "perspectiveinstance.h"

#include <QtCore/QDebug>

#include "perspective.h"
#include "perspective_p.h"
#include "guicontroller.h"
#include "iviewfactory.h"
#include "iview.h"

namespace GuiSystem {

class PerspectiveInstancePrivate
{
public:
    Perspective *perspective;
    QList<IView *> views;
    QMap<QString, IView *> mapToView;
};

} // namespace GuiSystem

using namespace GuiSystem;

PerspectiveInstance::PerspectiveInstance(QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectiveInstancePrivate)
{
}

PerspectiveInstance::~PerspectiveInstance()
{
    delete d_ptr;
}

Perspective * PerspectiveInstance::perspective() const
{
    return d_func()->perspective;
}

void PerspectiveInstance::setPerspective(Perspective *perspective)
{
    d_func()->perspective = perspective;
}

QList<IView *> PerspectiveInstance::views()
{
    return d_func()->views;
}

void PerspectiveInstance::addView(IView *view)
{
    Q_D(PerspectiveInstance);

    if (!d->views.contains(view)) {
        d->views.append(view);

        IViewFactory *factory = qobject_cast<IViewFactory *>(view->parent());
        Q_ASSERT_X(factory, "PerspectiveInstance::addView", "View's parent is not a factory.");

        d->mapToView.insert(factory->id(), view);
    }
}

void PerspectiveInstance::removeView(IView *view)
{
    Q_D(PerspectiveInstance);

    if (d->views.contains(view)) {
        d->views.removeAll(view);

        IViewFactory *factory = qobject_cast<IViewFactory *>(view->parent());
        Q_ASSERT_X(factory, "PerspectiveInstance::addView", "View's parent is not a factory.");

        d->mapToView.remove(factory->id());
    }
}

IView * PerspectiveInstance::view(const QString &id) const
{
    return d_func()->mapToView.value(id);
}
