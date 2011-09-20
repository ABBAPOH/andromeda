#include "navigationpanelview.h"

#include <core.h>
#include <perspectiveinstance.h>

#include "filemanagerview.h"
#include "navigationmodel.h"
#include "navigationpanel.h"

using namespace FileManagerPlugin;

NavigationPanelView::NavigationPanelView(QObject *parent) :
    GuiSystem::IView(parent)
{
    m_widget = new NavigationPanel;
    m_widget->setModel(ExtensionSystem::PluginManager::instance()->object<NavigationModel>("navigationModel"));
}

void NavigationPanelView::initialize()
{
    IView *view = perspectiveInstance()->view("FileManager");

    connect(m_widget, SIGNAL(triggered(QString)),
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
