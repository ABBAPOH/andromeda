#ifndef PERSPECTIVEWIDGET_H
#define PERSPECTIVEWIDGET_H

#include "guisystem_global.h"

#include <QtGui/QWidget>

class QSettings;

namespace GuiSystem {

class CentralWidget;
class Perspective;
class PerspectiveInstance;

class PerspectiveWidgetPrivate;
class GUISYSTEM_EXPORT PerspectiveWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PerspectiveWidget)
    Q_DISABLE_COPY(PerspectiveWidget)

public:
    explicit PerspectiveWidget(QWidget *parent = 0);
    ~PerspectiveWidget();

    PerspectiveInstance * instance() const;

    Perspective *perspective() const;
    void openPerspective(const QString &perspective);
    void closePerspective();

    void restoreSession(QSettings &s);
    void saveSession(QSettings &s);

protected:
    void createInstance(Perspective * p, int index);

    PerspectiveWidgetPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // PERSPECTIVEWIDGET_H
