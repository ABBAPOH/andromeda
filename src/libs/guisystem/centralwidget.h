#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtGui/QWidget>

namespace GuiSystem {

class IView;

class CentralWidgetPrivate;
class CentralWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralWidget)
public:
    explicit CentralWidget(QWidget *parent = 0);
    ~CentralWidget();

    void setView(IView *view);

signals:

public slots:

protected:
    CentralWidgetPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // CENTRALWIDGET_H
