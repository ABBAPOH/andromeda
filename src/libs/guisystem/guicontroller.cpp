#include "guicontroller.h"

#include <QtGui/QApplication>
#include <QtCore/QMap>
#include <QtCore/QDebug>

#include "iviewfactory.h"
#include "perspective.h"

namespace GuiSystem {

class GuiControllerPrivate
{
public:
    QMap<QString, IViewFactory *> factories;
    QMap<QString, Perspective *> perspectives;
};

}

using namespace GuiSystem;

GuiController::GuiController(QObject *parent) :
    QObject(parent),
    d_ptr(new GuiControllerPrivate)
{
}

GuiController::~GuiController()
{
    delete d_ptr;
}

GuiController * GuiController::instance()
{
    Q_ASSERT_X(qApp, "GuiController::instance", "Must construct QApplication before calling this method");

    static GuiController *m_instance = new GuiController(qApp);
    return m_instance;
}

void GuiController::addFactory(IViewFactory *factory)
{
    if (!factory)
        return;

    Q_D(GuiController);

    QString id = factory->id();
    if (d->factories.contains(id)) {
        qWarning() << "GuiController already contains factory with id" << id;
        delete d->factories.value(id);
    }
    d->factories.insert(id, factory);
}

IViewFactory * GuiController::factory(const QString &id) const
{
    Q_D(const GuiController);

    return d->factories.value(id);
}

void GuiController::addPerspective(Perspective *perspective)
{
    if (!perspective)
        return;

    Q_D(GuiController);

    QString id = perspective->id();
    if (d->perspectives.contains(id)) {
        qWarning() << "GuiController already contains perspective with id" << id;
    }
    d->perspectives.insert(id, perspective);
    perspective->setParent(this);
}

Perspective * GuiController::perspective(const QString &id) const
{
    Q_D(const GuiController);

    return d->perspectives.value(id);
}
