#include "viewoptions.h"

#include <QtCore/QSharedData>

namespace GuiSystem {

class ViewOptionsData : public QSharedData {
public:
    QVariantHash options;
};

} // namespace GuiSystem

using namespace GuiSystem;

ViewOptions::ViewOptions() :
    data(new ViewOptionsData)
{
}

ViewOptions::ViewOptions(const QString &id) :
    data(new ViewOptionsData)
{
    setId(id);
}

ViewOptions::ViewOptions(const QString &id, int area, int width, int height) :
    data(new ViewOptionsData)
{
    setId(id);
    setArea(area);

    if (width)
        setWidth(width);

    if (height)
        setHeight(height);
}

ViewOptions::~ViewOptions()
{
}

ViewOptions::ViewOptions(const ViewOptions &other) :
    data(other.data)
{
}

ViewOptions &ViewOptions::operator=(const ViewOptions &other)
{
    if (this != &other)
        data.operator=(other.data);
    return *this;
}

QString ViewOptions::id() const
{
    return data->options.value("id").toString();
}

void ViewOptions::setId(const QString &id)
{
    data->options["id"] = id;
}

int ViewOptions::area() const
{
    return data->options.value("area").toInt();
}

void ViewOptions::setArea(int area)
{
    data->options["area"] = area;
}

int ViewOptions::width() const
{
    return data->options.value("width").toInt();
}

void ViewOptions::setWidth(int width)
{
    data->options["width"] = width;
}

int ViewOptions::height() const
{
    return data->options.value("height").toInt();
}

void ViewOptions::setHeight(int height)
{
    data->options["height"] = height;
}

QVariant ViewOptions::option(const QString &option) const
{
    return data->options.value(option);
}

void ViewOptions::setOption(const QString &option, const QVariant &value)
{
    data->options.insert(option, value);
}

QVariantHash ViewOptions::toHash() const
{
    return data->options;
}
