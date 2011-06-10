#include "perspectiveinstance.h"

#include <QtCore/QDebug>

#include "perspective.h"
#include "guicontroller.h"
#include "iviewfactory.h"
#include "iview.h"

namespace GuiSystem {

class PerspectiveInstancePrivate
{
public:
    Perspective *perspective;
    QHash<QString, IView *> mapToView;
    int index;
};

} // namespace GuiSystem

using namespace GuiSystem;

PerspectiveInstance::PerspectiveInstance(QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectiveInstancePrivate)
{
    d_func()->index = -1;
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
//    if (d->mapToView.contains(id)) {
        d->mapToView.remove(id);
//    }
}

void PerspectiveInstance::removeView(const QString &id)
{
    d_func()->mapToView.remove(id);
}

IView * PerspectiveInstance::view(const QString &id) const
{
    return d_func()->mapToView.value(id);
}

int PerspectiveInstance::index() const
{
    return d_func()->index;
}

void PerspectiveInstance::setIndex(int index)
{
    d_func()->index = index;
}
