#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class IIOHandler;
class Perspective;
class IViewFactory;
class GuiControllerPrivate;
class GUISYSTEM_EXPORT GuiController : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(GuiController)
public:
    explicit GuiController(QObject *parent = 0);
    ~GuiController();

    static GuiController *instance();

    void addFactory(IViewFactory *factory);
    void removeFactory(IViewFactory *factory);
    void removeFactory(const QString &id);
    IViewFactory *factory(const QString &id) const;

    IIOHandler *handler(const QByteArray &format) const;
    QList<IIOHandler *> handlers() const;
    void addHandler(IIOHandler *handler);
    void removeHandler(IIOHandler *handler);

    void addPerspective(Perspective *perspective);
    Perspective *perspective(const QString &id) const;

    QStringList perspectiveIds() const;

signals:
    void factoryRemoved(const QString &id);

public slots:

protected:
    GuiControllerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // GUICONTROLLER_H
