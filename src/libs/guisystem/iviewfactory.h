#ifndef IVIEWFACTORY_H
#define IVIEWFACTORY_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>

class QObject;

namespace GuiSystem {

class State;
class IView;
class IViewFactoryPrivate;
class GUISYSTEM_EXPORT IViewFactory : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IViewFactory)
public:
    enum ShareMode { Clone = 0, ShareState = 1, ShareWindow = 2 };

    explicit IViewFactory(QObject * parent = 0);
    virtual ~IViewFactory();

    virtual QString id() const = 0;
    virtual QString type() const { return ""; }

    virtual ShareMode shareMode() const { return Clone; }

    IView *view(State *state);

protected:
    virtual IView *createView() const = 0;

    IViewFactoryPrivate *d_ptr; // TODO: maybe move map here?
};

} // namespace GuiSystem

#endif // IVIEWFACTORY_H
