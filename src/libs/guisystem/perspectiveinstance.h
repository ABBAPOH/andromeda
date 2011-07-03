#ifndef PERSPECTIVEINSTANCE_H
#define PERSPECTIVEINSTANCE_H

#include "guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class IView;
class Perspective;
class PerspectiveInstancePrivate;
class GUISYSTEM_EXPORT PerspectiveInstance : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PerspectiveInstance)
    Q_DISABLE_COPY(PerspectiveInstance)
public:
    explicit PerspectiveInstance(QObject *parent = 0);
    explicit PerspectiveInstance(PerspectiveInstance *parent);
    ~PerspectiveInstance();

    Perspective *perspective() const;
    void setPerspective(Perspective *perspective);

    PerspectiveInstance *parentInstance() const;

    QList<IView *> views();
    IView *view(const QString &id) const;

protected:
    PerspectiveInstancePrivate *d_ptr;
};

} // namespace GuiSystem

#endif // PERSPECTIVEINSTANCE_H
