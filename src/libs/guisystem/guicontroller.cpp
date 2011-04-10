#include "guicontroller.h"

#include <QtGui/QApplication>
#include <QtCore/QMap>
#include <QtCore/QDebug>

#include "iiohandler.h"
#include "iviewfactory.h"
#include "perspective.h"

namespace GuiSystem {

class GuiControllerPrivate
{
public:
    QMap<QString, IViewFactory *> factories;
    QMap<QString, Perspective *> perspectives;

    QMap<QByteArray, IIOHandler *> handlers;
};

}

using namespace GuiSystem;

GuiController::GuiController(QObject *parent) :
    QObject(parent),
    d_ptr(new GuiControllerPrivate)
{
    Perspective *perspective = new Perspective("empty", this);
    addPerspective(perspective);
}

GuiController::~GuiController()
{
    Q_D(GuiController);

    qDeleteAll(d->handlers.values());

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

void GuiController::removeFactory(IViewFactory *factory)
{
    if (!factory)
        return;

    removeFactory(factory->id());
}

void GuiController::removeFactory(const QString &id)
{
    Q_D(GuiController);

    if (d->factories.contains(id)) {
        d->factories.remove(id);
        emit factoryRemoved(id);
    }
}

IViewFactory * GuiController::factory(const QString &id) const
{
    Q_D(const GuiController);

    return d->factories.value(id);
}

void GuiController::addHandler(IIOHandler *handler)
{
    Q_D(GuiController);

    QByteArray format = handler->format();
    if (!d->handlers.contains(format))
        d->handlers.insert(format, handler);
}

IIOHandler * GuiController::handler(const QByteArray &format) const
{
    Q_D(const GuiController);

    return d->handlers.value(format);
}

QList<IIOHandler *> GuiController::handlers() const
{
    Q_D(const GuiController);

    return d->handlers.values();
}

void GuiController::removeHandler(IIOHandler *handler)
{
    Q_D(GuiController);

    d->handlers.remove(handler->format());
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

QStringList GuiController::perspectiveIds() const
{
    Q_D(const GuiController);

    return d->perspectives.keys();
}
