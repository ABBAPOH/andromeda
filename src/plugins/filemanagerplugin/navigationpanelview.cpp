#include "navigationpanelview.h"

#include <perspectiveinstance.h>

#include "filemanagerview.h"
#include "navigationpanel.h"

using namespace FileManagerPlugin;

NavigationPanelView::NavigationPanelView(QObject *parent) :
    GuiSystem::IView(parent)
{
    m_widget = new NavigationPanel;
}

void NavigationPanelView::initialize()
{
    IView *view = perspectiveInstance()->view("FileManager");

    connect(m_widget, SIGNAL(currentPathChanged(QString)),
            view->widget(), SLOT(setCurrentPath(QString)));
}

QWidget * NavigationPanelView::widget() const
{
    return m_widget;
}

QString NavigationPanelFactory::id() const
{
    return QLatin1String("NavigationPanel");
}

QString NavigationPanelFactory::type() const
{
    return QLatin1String("FileManager");
}

GuiSystem::IView * FileManagerPlugin::NavigationPanelFactory::createView()
{
    return new NavigationPanelView(this);
}
