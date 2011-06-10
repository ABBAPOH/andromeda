#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include <QtGui/QWidget>

namespace GuiSystem {

class CentralWidgetPrivate;
class CentralWidget : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CentralWidget)
    Q_DISABLE_COPY(CentralWidget)
public:
    enum Area { Invalid = -1, Left = 0, Top = 1, Right = 2, Bottom = 3, Central = 4 };

    CentralWidget(QWidget *parent = 0);
    ~CentralWidget();

    void addWidget(QWidget *widget, int area);

    void hideArea(int area);
    void showArea(int area);
    bool isAreaVisible(int area);

signals:

protected:
    void resizeEvent(QResizeEvent *);

    CentralWidgetPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // CENTRALWIDGET_H
