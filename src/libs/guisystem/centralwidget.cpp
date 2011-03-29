#include "centralwidget.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QToolBar>

#include "iview.h"

namespace GuiSystem {

class CentralWidgetPrivate
{
public:
    IView *view;
    QVBoxLayout *layout;
};

} // namespace GuiSystem

using namespace GuiSystem;

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CentralWidgetPrivate)
{
    Q_D(CentralWidget);

    d->view = 0;
    d->layout = new QVBoxLayout(this);
    d->layout->setMargin(0);
    d->layout->setSpacing(0);
}

CentralWidget::~CentralWidget()
{
    delete d_ptr;
}

void CentralWidget::setView(IView *view)
{
    Q_D(CentralWidget);

    d->view = view;
    QLayoutItem *child;
    while ( (child = d->layout->takeAt(0)) != 0 ) {
        child->widget()->hide();
        delete child;
    }
    d->layout->addWidget(view->toolBar());
    d->layout->addWidget(view->widget());
    view->toolBar()->show();
    view->widget()->show();
}
