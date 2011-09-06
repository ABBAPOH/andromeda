#ifndef IVIEW_H
#define IVIEW_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QWidget>

class QWidget;
class QToolBar;

namespace GuiSystem {

class PerspectiveInstance;
class IViewFactory;

class IViewPrivate;
class GUISYSTEM_EXPORT IView : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(IView)
    Q_DISABLE_COPY(IView)

public:
    explicit IView(QObject *parent = 0);
    virtual ~IView();

    virtual void initialize() {}
    virtual void shutdown() {}

    virtual QWidget *widget() const = 0;
    virtual QToolBar *toolBar() const { return 0; }

    QString id() const;
    QString type() const;

    QWidget *container() const;
    void setContainer(QWidget *widget);

    PerspectiveInstance *perspectiveInstance() const;
    void setPerspectiveInstance(PerspectiveInstance *state);

protected:
    void setFactory(IViewFactory *factory);

protected:
    IViewPrivate *d_ptr;

    friend class IViewFactory;
};

class ViewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewWidget(QWidget *parent = 0);
    ~ViewWidget();

    void setView(IView *view);

    QToolBar *toolBar() const;
    QWidget *widget() const;

protected:
    IView *m_view;
};

} // namespace GuiSystem

#endif // IVIEW_H
