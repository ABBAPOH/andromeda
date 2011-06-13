#include "perspective.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMap>

#include "guicontroller.h"
#include "iiohandler.h"
#include "perspectiveinstance.h"

namespace GuiSystem {

class PerspectivePrivate
{
public:
    PerspectivePrivate() : parent(0) {}
    QString id;
    QString name;
    QStringList types;
    Perspective *parent;

    QMap<QString, ViewOptions> views;
};

} // namespace GuiSystem

using namespace GuiSystem;

Perspective::Perspective(QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectivePrivate)
{
}

Perspective::Perspective(const QString &id, QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectivePrivate)
{
    setId(id);
}

Perspective::Perspective(const QString &id, Perspective *parent) :
    QObject(parent),
    d_ptr(new PerspectivePrivate)
{
    setId(id);

    d_func()->parent = parent;
}

Perspective::Perspective(const QString &id, const QStringList &types, QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectivePrivate)
{
    Q_D(Perspective);

    d->id = id;

    setTypes(types);
}

Perspective::~Perspective()
{
    delete d_ptr;
}

QString Perspective::id() const
{
    return d_func()->id;
}

void Perspective::setId(const QString &id)
{
    d_func()->id = id;
}

QString Perspective::name() const
{
    return d_func()->name;
}

void Perspective::setName(const QString &name)
{
    d_func()->name = name;
}

Perspective * Perspective::parentPerspective() const
{
    return d_func()->parent;
}

QStringList Perspective::types() const
{
    return d_func()->types;
}

void Perspective::setTypes(const QStringList &types)
{
    d_func()->types = types;
}

void Perspective::addType(const QString &type)
{
    Q_D(Perspective);

    if (!d->types.contains(type))
        d->types.append(type);
}

void Perspective::load(const QString &file)
{
    QList<IIOHandler *> handlers = GuiController::instance()->handlers();
    IIOHandler *handler = 0;

    for (int i = 0; i < handlers.size(); i++) {
        if (handlers[i]->canHandle(file)) {
            handler = handlers[i];
            break;
        }
    }

    if (!handler)
        return;

    handler->read(file, this);
}

void Perspective::save(const QString &file, const QByteArray format)
{
    IIOHandler *handler = 0;
    if (!format.isEmpty()) {
        handler = GuiController::instance()->handler(format);
    } else {
        handler = GuiController::instance()->handler(QFileInfo(file).suffix().toUtf8());
    }

    if (!handler)
        return;

    if (handler->canWrite()) {
        handler->write(file, this);
    }
}

void Perspective::addView(const ViewOptions &options)
{
    Q_D(Perspective);

    d->views.insert(options.id(), options);
}

void Perspective::addView(const QString &id, int area, int width, int height)
{
    ViewOptions options(id, area, width, height);

    addView(options);
}

QStringList Perspective::views() const
{
    Q_D(const Perspective);

    return d->views.keys();
}

ViewOptions Perspective::viewOptions(const QString &id) const
{
    Q_D(const Perspective);

    return d->views.value(id);
}

