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
    QList<IView *> views;
};

} // namespace GuiSystem

using namespace GuiSystem;

PerspectiveInstance::PerspectiveInstance(Perspective *perspective) :
    QObject(perspective),
    d_ptr(new PerspectiveInstancePrivate)
{
    Q_D(PerspectiveInstance);

    PerspectivePrivate *pd = perspective->d_func();
    GuiController *controller = GuiController::instance();

    QStringList ids = pd->views.keys();
    qDebug() << ids;
    for (int i = 0; i < ids.size(); i++) {
        QString id = ids[i];
        IViewFactory *factory = controller->factory(id);
        if (!factory) {
            qWarning() << "Can't find view with id" << id;
            continue;
        }
        // FIXME: always clone views
        IView *view = factory->createView();
        view->setOptions(pd->views.value(id));
        d->views.append(view);
#warning "STOPPED HERE"
    }
}

PerspectiveInstance::~PerspectiveInstance()
{
    delete d_ptr;
}

QList<IView *> PerspectiveInstance::views()
{
    return d_func()->views;
}
