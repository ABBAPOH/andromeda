#include "state.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>

#include "guicontroller.h"
#include "perspective.h"
#include "perspectiveinstance.h"

namespace GuiSystem {

class StatePrivate
{
public:
    QHash<QString, PerspectiveInstance*> instances; // perspective id -> instance
    QString currentInstanceId;
};

} // namespace GuiSystem

using namespace GuiSystem;

State::State(QObject *parent) :
    QObject(parent),
    d_ptr(new StatePrivate)
{
}

State::~State()
{
    delete d_ptr;
}

PerspectiveInstance * State::currentInstance() const
{
    Q_D(const State);

    return d->instances.value(d->currentInstanceId);
}

void State::setCurrentPerspectiveId(const QString &id)
{
    Q_D(State);

    PerspectiveInstance *instance = d->instances.value(id);
    if (!instance) {
        Perspective *perspective = GuiController::instance()->perspective(id);
        if (!perspective) {
            qWarning() << "State::setCurrentInstanceId: Can't find perspective with id" << id;
            return;
        }
        instance = perspective->instance();
        d->instances.insert(id, instance);
    }
    d->currentInstanceId = id;
}

// TODO: Implement. Need to change current instance.
//void State::closeInstance(const QString &id)
//{
//    Q_D(State);
//    delete d->instances.value(id);
//    d->instances.remove(id);
//}

QStringList State::perspectiveIds() const
{
    Q_D(const State);

    return d->instances.keys();
}
