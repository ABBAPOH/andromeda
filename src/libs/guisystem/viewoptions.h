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
    ViewOptions();
    explicit ViewOptions(const QString &id);
    ViewOptions(const QString &id, int area, int width = 0, int height = 0);

    ViewOptions(const ViewOptions &);
    ViewOptions &operator=(const ViewOptions &);
    ~ViewOptions();

    QString id() const;
    void setId(const QString &id);

    int area() const;
    void setArea(int area);

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

    QVariant option(const QString &option) const;
    void setOption(const QString &option, const QVariant &value);

    QVariantHash toHash() const;

private:
    QSharedDataPointer<ViewOptionsData> data;
};

} // namespace GuiSystem

#endif // VIEWOPTIONS_H
