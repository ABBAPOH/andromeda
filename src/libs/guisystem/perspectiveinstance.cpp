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
    QHash<QString, IView *> mapToView;
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
    return d_func()->mapToView.values();
}

void PerspectiveInstance::addView(IView *view)
{
    Q_D(PerspectiveInstance);

    QString id = view->factoryId();
    if (!d->mapToView.contains(id)) {
        d->mapToView.insert(id, view);
    }
}

void PerspectiveInstance::removeView(IView *view)
{
    Q_D(PerspectiveInstance);

    QString id = view->factoryId();
    if (d->mapToView.contains(id)) {
        d->mapToView.remove(id);
    }
}

IView * PerspectiveInstance::view(const QString &id) const
{
    return d_func()->mapToView.value(id);
}
