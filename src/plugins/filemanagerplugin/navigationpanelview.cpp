#include "navigationpanelview.h"

#include <state.h>

#include "filemanagerview.h"
#include "navigationpanel.h"

using namespace FileManagerPlugin;

NavigationPanelView::NavigationPanelView(QObject *parent) :
    GuiSystem::IView(parent)
{
    m_widget = new NavigationPanel;
}

void NavigationPanelView::initialize(GuiSystem::State *state)
{
    IView *view = state->view("FileManager");

//    FileManagerView *fileManagerView = qobject_cast<FileManagerView *>(view);
    connect(m_widget, SIGNAL(folderClicked(QString)),
            view->widget(), SLOT(setCurrentPath(QString)));
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
