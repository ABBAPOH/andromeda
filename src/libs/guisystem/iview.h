#ifndef IVIEW_H
#define IVIEW_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>

#include "viewoptions.h"

class QWidget;
class QToolBar;

namespace GuiSystem {

class State;
class GUISYSTEM_EXPORT IView : public QObject
{
    Q_OBJECT
public:
    IView() : QObject() {}
    virtual ~IView() {}

    virtual void initialize(State */*state*/) {}
    virtual void shutdown() {}

    virtual QString type() const = 0;

    virtual QWidget *widget() const = 0;
    virtual QToolBar *toolBar() const { return 0; }

    virtual void setOptions(const ViewOptions &options)
    {
        m_area = options.area();
        m_options = options;
    }

    ViewOptions options() const { return m_options; }

    // FIXME: changebal area when switching tabs
    int area() const
    {
        return m_area;
    }

private:
    // FIXME: remove area
    int m_area;
    ViewOptions m_options;
};

} // namespace GuiSystem

#endif // IVIEW_H
