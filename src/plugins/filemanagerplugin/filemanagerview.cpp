#include "filemanagerview.h"

#include "filemanagerwidget.h"

#include <state.h>

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IHistoryView(parent)
{
    m_widget = new FileManagerWidget();
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SLOT(onCurrentPathChange(QString)));
}

void FileManagerView::initialize(/*GuiSystem::State *state*/)
{
    const GuiSystem::State *state = this->state();
//    QString path = state->property("path").toString();
    QString path = state->object("stateController")->property("currentPath").toString();
    m_state = state;
    m_widget->setCurrentPath(path);
}

QString FileManagerView::type() const
{
    return "filemanager";
}

QWidget * FileManagerView::widget() const
{
    return m_widget;
}

QString FileManagerFactory::id() const
{
    return "FileManager";
}

GuiSystem::IView * FileManagerPlugin::FileManagerFactory::createView() const
{
    return new FileManagerView;
}

void FileManagerView::back()
{
    m_widget->back();
}

void FileManagerView::forward()
{
    m_widget->forward();
}

void FileManagerView::onCurrentPathChange(const QString &path)
{
    QObject *controller = state()->object("stateController");
    if (controller->property("currentPath").toString() != path)
        controller->setProperty("currentPath", path);
//    if (m_state->property("path").toString() != path)
//        m_state->setProperty("path", path);
}
