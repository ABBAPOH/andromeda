#include "navigationpanelview.h"

#include "navigationpanel.h"

using namespace FileManagerPlugin;

NavigationPanelView::NavigationPanelView(QObject *parent) :
    GuiSystem::IView(parent)
{
    m_widget = new NavigationPanel;
}

QString NavigationPanelView::type() const
{
    return "";
}

QWidget * NavigationPanelView::widget() const
{
    return m_widget;
}

QString NavigationPanelFactory::id() const
{
    return "NavigationPanel";
}

GuiSystem::IView * FileManagerPlugin::NavigationPanelFactory::createView() const
{
    return new NavigationPanelView;
}
