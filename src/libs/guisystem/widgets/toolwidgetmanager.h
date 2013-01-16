#ifndef TOOLWIDGETMANAGER_H
#define TOOLWIDGETMANAGER_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class ToolWidget;
class ToolWidgetFactory;

class ToolWidgetManagerPrivate;
class GUISYSTEM_EXPORT ToolWidgetManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ToolWidgetManager)
    Q_DISABLE_COPY(ToolWidgetManager)

public:
    explicit ToolWidgetManager(QObject *parent = 0);
    ~ToolWidgetManager();

    static ToolWidgetManager *instance();

    QList<ToolWidgetFactory *> factories() const;
    ToolWidgetFactory *factory(const QByteArray &id) const;
    void addFactory(ToolWidgetFactory *factory);
    void removeFactory(ToolWidgetFactory *factory);

private slots:
    void onFactoryDestroyed(QObject *);

protected:
    ToolWidgetManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // TOOLWIDGETMANAGER_H
