#include "perspectiveswitcher.h"

#include "guicontroller.h"
#include "perspective.h"
#include "state.h"

#include <QtGui/QAction>

namespace GuiSystem
{

class PerspectiveSwitcherPrivate
{
public:
    State *state;
};

}

using namespace GuiSystem;

PerspectiveSwitcher::PerspectiveSwitcher(QWidget *parent) :
    QToolBar(parent),
    d_ptr(new PerspectiveSwitcherPrivate)
{
}

PerspectiveSwitcher::~PerspectiveSwitcher()
{
    delete d_ptr;
}

State * PerspectiveSwitcher::state() const
{
    return d_func()->state;
}

void PerspectiveSwitcher::setState(State *state)
{
    Q_D(PerspectiveSwitcher);

    if (d->state == state)
        return;

    d->state = state;

    qDeleteAll(actions()); // right ??

    QStringList ids = state->availablePerspectives();
    for (int i = 0; i < ids.size(); i++) {
        QString id = ids[i];
        Perspective *perspective = GuiController::instance()->perspective(id);

        QAction *action = new QAction(perspective->name(), this);
        addAction(action);
        action->setData(perspective->id());
        connect(action, SIGNAL(triggered()), SLOT(onTrigger()));
    }
}

void PerspectiveSwitcher::onTrigger()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (!action)
        return; // or ASSERT?

    Q_D(PerspectiveSwitcher);

    QString id = action->data().toString();
    d->state->setCurrentPerspective(id);
}
