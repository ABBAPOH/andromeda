#ifndef TOOLWIDGETFACTORY_H
#define TOOLWIDGETFACTORY_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

namespace GuiSystem {

class ToolWidget;

class GUISYSTEM_EXPORT ToolWidgetFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ToolWidgetFactory)

public:
    enum Area { LeftArea = 0x1,
                RightArea = 0x2,
                TopArea = 0x4,
                BottomArea = 0x8
              };
    Q_ENUMS(Area)

    explicit ToolWidgetFactory(const QByteArray &id, QObject *parent = 0);

    QByteArray id() const;

    virtual Area defaultArea() const;
    virtual ToolWidget *createToolWidget(QWidget *parent = 0) const = 0;

private:
    const QByteArray m_id;
};

} // namespace GuiSystem

#endif // TOOLWIDGETFACTORY_H
