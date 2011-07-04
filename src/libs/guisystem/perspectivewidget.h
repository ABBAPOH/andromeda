#ifndef PERSPECTIVEWIDGET_H
#define PERSPECTIVEWIDGET_H

#include <QtGui/QWidget>
#include <QHash>

class QStackedLayout;

namespace GuiSystem {

class CentralWidget;
class Perspective;
class PerspectiveInstance;

class PerspectiveWidgetPrivate;
class PerspectiveWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PerspectiveWidget)
    Q_DISABLE_COPY(PerspectiveWidget)
public:
    explicit PerspectiveWidget(QWidget *parent = 0);
    ~PerspectiveWidget();

    PerspectiveInstance * currentInstance() const;

    Perspective *currentPerspective() const;
    void openPerspective(const QString &perspective);
    void closePerspective(const QString &perspective);

protected:
    void createInstance(Perspective * p, int index);

    PerspectiveWidgetPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // PERSPECTIVEWIDGET_H
