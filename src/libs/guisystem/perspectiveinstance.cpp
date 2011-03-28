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

    if (!d->views.contains(view))
        d->views.append(view);
}
