#ifndef VIEWOPTIONS_H
#define VIEWOPTIONS_H

#include "guisystem_global.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QVariant>

namespace GuiSystem {

class ViewOptionsData;

class GUISYSTEM_EXPORT ViewOptions
{
public:
    explicit ViewOptions(int area = 0);

    ViewOptions(const ViewOptions &);
    ViewOptions &operator=(const ViewOptions &);
    ~ViewOptions();

    int area() const;

    QVariant option(const QString &option) const;
    void setOption(const QString &option, const QVariant &value);

private:
    QSharedDataPointer<ViewOptionsData> data;
};

} // namespace GuiSystem

#endif // VIEWOPTIONS_H
