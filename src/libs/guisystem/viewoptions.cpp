#include "viewoptions.h"

#include <QtCore/QSharedData>

namespace GuiSystem {

class ViewOptionsData : public QSharedData {
public:
    int area;
    QVariantHash options;
};

} // namespace GuiSystem

using namespace GuiSystem;

ViewOptions::ViewOptions(int area) : data(new ViewOptionsData)
{
    data->area = area;
}

ViewOptions::ViewOptions(const ViewOptions &rhs) : data(rhs.data)
{
}

ViewOptions &ViewOptions::operator=(const ViewOptions &rhs)
{
    if (this != &rhs)
        data.operator=(rhs.data);
    return *this;
}

ViewOptions::~ViewOptions()
{
}

int GuiSystem::ViewOptions::area() const
{
    return data->area;
}

QVariant ViewOptions::option(const QString &option) const
{
    return data->options.value(option);
}

void ViewOptions::setOption(const QString &option, const QVariant &value)
{
    data->options.insert(option, value);
}
