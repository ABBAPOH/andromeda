#include "perspective.h"

#include <QtCore/QFileInfo>
#include <QtCore/QMap>
#include <QtCore/QVariant>

#include "guicontroller.h"
#include "iiohandler.h"
#include "perspectiveinstance.h"

namespace GuiSystem {

struct Data
{
    QString id;
    int area;
    QVariantMap properties;
};

class PerspectivePrivate
{
public:
    PerspectivePrivate() : parent(0) {}
    QString id;
    QIcon icon;
    QString name;
    Perspective *parent;

    QMap<QString, Data*> views;
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

Perspective::~Perspective()
{
    qDeleteAll(d_func()->views);
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

QIcon Perspective::icon() const
{
    return d_func()->icon;
}

void Perspective::setIcon(const QIcon &icon)
{
    d_func()->icon = icon;
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

void Perspective::addView(const QString &id, int area)

{
    Q_D(Perspective);

    Data *data = new Data;
    data->id = id;
    data->area = area;
    d->views.insert(id, data);
}

void Perspective::removeView(const QString &id)
{
    Q_D(Perspective);

    Data * data = d->views.take(id);
    if (data) {
        delete data;
    }
}

QStringList Perspective::views() const
{
    Q_D(const Perspective);

    return d->views.keys();
}

int Perspective::viewArea(const QString &id) const
{
    Q_D(const Perspective);

    return d->views.value(id)->area;
}

QVariant Perspective::viewProperty(const QString &id, const QString &property) const
{
    Q_D(const Perspective);

    return d->views.value(id)->properties.value(property);
}

bool Perspective::load(const QString &file)
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
        return false;

    return handler->read(file, this);
}

bool Perspective::save(const QString &file, const QByteArray &format)
{
    IIOHandler *handler = 0;
    if (!format.isEmpty()) {
        handler = GuiController::instance()->handler(format);
    } else {
        handler = GuiController::instance()->handler(QFileInfo(file).suffix().toUtf8());
    }

    if (!handler || !handler->canWrite())
        return false;

    return handler->write(file, this);
}
