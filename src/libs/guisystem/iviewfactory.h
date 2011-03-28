#ifndef IVIEWFACTORY_H
#define IVIEWFACTORY_H

#include "guisystem_global.h"

#include <QtCore/QString>

namespace GuiSystem {

class IView;
class GUISYSTEM_EXPORT IViewFactory
{
public:
    enum ShareMode { Share = 0, Clone = 1, Preffered = 2 };

    IViewFactory() {}
    virtual ~IViewFactory() {}

    virtual QString id() const = 0;
    virtual QString type() const { return ""; }
    virtual IView *createView() const = 0;

    virtual ShareMode shareMode() const { return Clone; }
};

} // namespace GuiSystem

#endif // IVIEWFACTORY_H
