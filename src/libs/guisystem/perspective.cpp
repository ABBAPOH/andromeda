#include "perspective.h"
#include "perspective_p.h"

#include "perspectiveinstance.h"

using namespace GuiSystem;

Perspective::Perspective(const QString &id, QObject *parent) :
    QObject(parent),
    d_ptr(new PerspectivePrivate)
{
    Q_D(Perspective);

    d->id = id;
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

QString Perspective::name() const
{
    return d_func()->name;
}

void Perspective::setName(const QString &name)
{
    d_func()->name = name;
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

