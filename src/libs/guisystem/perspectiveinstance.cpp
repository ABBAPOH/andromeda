#include "perspectiveinstance.h"

#include <QtCore/QDebug>

#include "perspective.h"
#include "guicontroller.h"
#include "iviewfactory.h"
#include "iview.h"

namespace GuiSystem {

class PerspectiveInstancePrivate
{
    Q_DECLARE_PUBLIC(PerspectiveInstance)
    PerspectiveInstance *q_ptr;
public:
    PerspectiveInstancePrivate(PerspectiveInstance *qq) :
        q_ptr(qq),
        perspective(0),
        parentInstance(0)
    {}

    Perspective *perspective;
    QHash<QString, IView *> mapToView;
    PerspectiveInstance *parentInstance;

    void createViews();
    void initializeViews();

// deprecated:
    int index;
};

} // namespace GuiSystem

using namespace GuiSystem;

void PerspectiveInstancePrivate::createViews()
{
    Q_Q(PerspectiveInstance);

    GuiController *controller = GuiController::instance();

    QStringList ids = perspective->views();
    for (int i = 0; i < ids.size(); i++) {
        QString id = ids[i];
        IViewFactory *factory = controller->factory(id);
        if (!factory) {
            qWarning() << "Can't find view factory with id" << id;
            continue;
        }

        IView *view = factory->view();
        mapToView.insert(id, view);
        view->setPerspectiveInstance(q);
        view->setOptions(perspective->viewOptions(id));
    }
}

void PerspectiveInstancePrivate::initializeViews()
{
    foreach (IView *view, mapToView) {
        view->initialize();
    }
}

PerspectiveInstance::PerspectiveInstance(QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectiveInstancePrivate(this))
{
}

PerspectiveInstance::PerspectiveInstance(PerspectiveInstance *parent) :
    QObject(parent),
    d_ptr(new PerspectiveInstancePrivate(this))
{
    if (parent)
        d_func()->parentInstance = parent;
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
    Q_D(PerspectiveInstance);

    if (d->perspective == perspective)
        return;

    d->perspective = perspective;
    d->createViews();
    d->initializeViews();
}

PerspectiveInstance * PerspectiveInstance::parentInstance() const
{
    return d_func()->parentInstance;
}

QList<IView *> PerspectiveInstance::views()
{
    return d_func()->mapToView.values();
}

IView * PerspectiveInstance::view(const QString &id) const
{
    IView *result = d_func()->mapToView.value(id);
    if (result)
        return result;

    return parentInstance() ? parentInstance()->view(id) : 0;
}
