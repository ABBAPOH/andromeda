#include "perspectivewidget.h"

#include <QtCore/QEvent>
#include <QtGui/QStackedLayout>

#include "centralwidget.h"
#include "guicontroller.h"
#include "iview.h"
#include "perspective.h"
#include "perspectiveinstance.h"

using namespace GuiSystem;

static PerspectiveWidget *m_activeWidget = 0;

PerspectiveWidget::PerspectiveWidget(QWidget *parent) :
    QWidget(parent),
    m_layout(new QStackedLayout(this)),
    m_currentInstance(0)
{
}

PerspectiveWidget *PerspectiveWidget::activeWidget()
{
    return m_activeWidget;
}

PerspectiveInstance * PerspectiveWidget::currentInstance() const
{
    return m_currentInstance;
}

Perspective * PerspectiveWidget::currentPerspective() const
{
    return m_currentInstance ? m_currentInstance->perspective() : 0;
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

#warning "TODO: hide and show areas properly"
void PerspectiveWidget::openPerspective(const QString &perspective)
{
    Perspective *p = GuiController::instance()->perspective(perspective);

    if (!p)
        return;

    if (this->currentPerspective() == p)
        return;

    QList<Perspective *> perspectives = parentPerspectives(p);
    perspectives.prepend(p);

    int index = -1;
    PerspectiveInstance *topInstance = m_mapToInstace.value(perspectives.first());
    if (!topInstance) {
        CentralWidget *widget = new CentralWidget(this);
        for (int i = CentralWidget::Left; i <= CentralWidget::Central; i++) {
            widget->hideArea(i);
        }
        index = m_layout->addWidget(widget);
    } else {
        index = m_topInstances.indexOf(topInstance);
    }

    for (int i = perspectives.size() - 1; i >= 0; --i) {
        createInstance(perspectives[i], index);
    }

    if (!topInstance) {
        topInstance = m_mapToInstace.value(perspectives.first());
        m_topInstances.append(topInstance);
    }
    m_currentInstance = m_mapToInstace.value(p);
    m_layout->setCurrentIndex(index);
}

void PerspectiveWidget::createInstance(Perspective *p, int index)
{
    if (m_mapToInstace.contains(p)) {
        return;
    }

    PerspectiveInstance *parent = m_mapToInstace.value(p->parentPerspective());
    PerspectiveInstance *instance = new PerspectiveInstance(parent);
    m_mapToInstace.insert(p, instance);

    instance->setPerspective(p);
    foreach (IView *view, instance->views()) {
        ViewWidget *viewWidget = new ViewWidget;
        viewWidget->setView(view);
        view->setContainer(viewWidget);

        CentralWidget *widget = static_cast<CentralWidget *>(m_layout->widget(index));
        widget->addWidget(viewWidget, view->area());
        widget->showArea(view->area());
    }
}

void PerspectiveWidget::hideViews()
{
    PerspectiveInstance *instance = m_currentInstance;
    while (instance) {
        foreach (IView * view, instance->views()) {
            view->container()->hide();
        }
        instance = instance->parentInstance();
    }
}

void PerspectiveWidget::showViews()
{
    PerspectiveInstance *instance = m_currentInstance;
    while (instance) {
        foreach (IView * view, instance->views()) {
            view->container()->show();
        }
        instance = instance->parentInstance();
    }
}

#include <QDebug>
bool GuiSystem::PerspectiveWidget::event(QEvent *e)
{
    if (e->type() == QEvent::WindowActivate || e->type() == QEvent::Show) {
        if (isActiveWindow()) {
            qDebug() << "changing current to" << this << this->isActiveWindow();
            m_activeWidget = this;
        }
    }
    return false;
}
