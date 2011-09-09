#include "perspectivewidget.h"

#include <QtCore/QEvent>
#include <QtCore/QHash>
#include <QtCore/QSettings>
#include <QtGui/QStackedLayout>

#include "centralwidget.h"
#include "guicontroller.h"
#include "iview.h"
#include "perspective.h"
#include "perspectiveinstance.h"

namespace GuiSystem
{

class PerspectiveWidgetPrivate
{
public:
    QStackedLayout *layout;
    QList<PerspectiveInstance *> topInstances;
    PerspectiveInstance * currentInstance;
    QHash<Perspective *, PerspectiveInstance*> mapToInstace;
};

} // namespace GuiSystem

using namespace GuiSystem;

PerspectiveWidget::PerspectiveWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new PerspectiveWidgetPrivate)
{
    Q_D(PerspectiveWidget);

    d->layout = new QStackedLayout(this);
    d->currentInstance = 0;
}

PerspectiveWidget::~PerspectiveWidget()
{
    delete d_ptr;
}

PerspectiveInstance * PerspectiveWidget::instance() const
{
    return d_func()->currentInstance;
}

Perspective * PerspectiveWidget::perspective() const
{
    Q_D(const PerspectiveWidget);

    return d->currentInstance ? d->currentInstance->perspective() : 0;
}

Perspective * topPerspective(Perspective *child)
{
    while (child->parentPerspective()) {
        child = child->parentPerspective();
    }
    return child;
}

QList<Perspective *> parentPerspectives(Perspective *child)
{
    QList<Perspective *> result;
    while (child->parentPerspective()) {
        child = child->parentPerspective();
        result.append(child);
    }
    return result;
}

void PerspectiveWidget::openPerspective(const QString &perspective)
{
    Q_D(PerspectiveWidget);

    Perspective *p = GuiController::instance()->perspective(perspective);

    if (!p)
        return;

    if (this->perspective() == p)
        return;

    QList<Perspective *> perspectives = parentPerspectives(p);
    perspectives.prepend(p);

    int index = -1;
    PerspectiveInstance *topInstance = d->mapToInstace.value(perspectives.first());
    if (!topInstance) {
        CentralWidget *widget = new CentralWidget(this);
// fixme: do not hide views that will be used
        for (int i = CentralWidget::Left; i <= CentralWidget::Central; i++) {
            widget->hideArea(i);
        }
        index = d->layout->addWidget(widget);
    } else {
        index = d->topInstances.indexOf(topInstance);
    }

    for (int i = perspectives.size() - 1; i >= 0; --i) {
        createInstance(perspectives[i], index);
    }

    if (!topInstance) {
        topInstance = d->mapToInstace.value(perspectives.first());
        d->topInstances.append(topInstance);
    }
    d->currentInstance = d->mapToInstace.value(p);
    d->layout->setCurrentIndex(index);
}

void PerspectiveWidget::closePerspective()
{
    // todo : impelement
}

void PerspectiveWidget::restoreSession(QSettings &s)
{
    QString id = s.value(QLatin1String("perspective")).toString();
    openPerspective(id);
    foreach (IView *view, instance()->views()) {
        s.beginGroup(view->id());
        view->restoreSession(s);
        s.endGroup();
    }
}

void PerspectiveWidget::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("perspective"), perspective()->id());
    foreach (IView *view, instance()->views()) {
        s.beginGroup(view->id());
        view->saveSession(s);
        s.endGroup();
    }
}

void PerspectiveWidget::createInstance(Perspective *p, int index)
{
    Q_D(PerspectiveWidget);

    if (d->mapToInstace.contains(p))
        return;

    PerspectiveInstance *parent = d->mapToInstace.value(p->parentPerspective());
    PerspectiveInstance *instance = new PerspectiveInstance(parent);
    instance->setPerspectiveWidget(this);
    d->mapToInstace.insert(p, instance);

    instance->setPerspective(p);
    foreach (IView *view, instance->views()) {
        ViewWidget *viewWidget = new ViewWidget;
        viewWidget->setView(view);
        view->setContainer(viewWidget);

        CentralWidget *widget = static_cast<CentralWidget *>(d->layout->widget(index));
        int area = p->viewArea(view->id());
        widget->addWidget(viewWidget, area);
        widget->showArea(area);
    }
}
