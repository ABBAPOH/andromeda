#include "centralwidget.h"

#include <QtGui/QResizeEvent>
#include <minisplitter.h>

namespace GuiSystem {

class CentralWidgetPrivate
{
public:
    MiniSplitter * verticalSplitter;
    MiniSplitter * centralSplitter;
    MiniSplitter * splitters[5];
};

} // namespace GuiSystem

using namespace GuiSystem;

static const int minArea = 0;
static const int maxArea = 4;

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent),
    d_ptr(new CentralWidgetPrivate)
{
    Q_D(CentralWidget);

    setObjectName("CentralWidget");

    d->splitters[Left] = new MiniSplitter;
    d->splitters[Left]->setOrientation(Qt::Vertical);
    d->splitters[Top] = new MiniSplitter;
    d->splitters[Top]->setOrientation(Qt::Horizontal);
    d->splitters[Right] = new MiniSplitter;
    d->splitters[Right]->setOrientation(Qt::Vertical);
    d->splitters[Bottom] = new MiniSplitter;
    d->splitters[Bottom]->setOrientation(Qt::Horizontal);
    d->splitters[Central] = new MiniSplitter;
    d->splitters[Central]->setOrientation(Qt::Vertical);

    d->centralSplitter = new MiniSplitter;
    d->centralSplitter->setOrientation(Qt::Horizontal);
    d->centralSplitter->addWidget(d->splitters[Left]);
    d->centralSplitter->addWidget(d->splitters[Central]);
    d->centralSplitter->addWidget(d->splitters[Right]);
    d->centralSplitter->setSizes(QList<int>() << 1 << 3 << 1);

    d->verticalSplitter = new MiniSplitter(this);
    d->verticalSplitter->setOrientation(Qt::Vertical);

    d->verticalSplitter->addWidget(d->splitters[Top]);
    d->verticalSplitter->addWidget(d->centralSplitter);
    d->verticalSplitter->addWidget(d->splitters[Bottom]);
    d->verticalSplitter->setSizes(QList<int>() << 1 << 3 << 1);
}

CentralWidget::~CentralWidget()
{
    delete d_ptr;
}

void CentralWidget::addWidget(QWidget *widget, int area)
{
    if (area < minArea || area > maxArea)
        return;

    d_func()->splitters[area]->addWidget(widget);
}

void CentralWidget::resizeEvent(QResizeEvent *e)
{
    d_func()->verticalSplitter->resize(e->size());
}

void CentralWidget::hideArea(int area)
{
    if (area < minArea || area > maxArea)
        return;

    d_func()->splitters[area]->hide();
}

void CentralWidget::showArea(int area)
{
    if (area < minArea || area > maxArea)
        return;

    d_func()->splitters[area]->show();
}

bool CentralWidget::isAreaVisible(int area)
{
    if (area < minArea || area > maxArea)
        return false;

    return d_func()->splitters[area]->isVisible();
}
