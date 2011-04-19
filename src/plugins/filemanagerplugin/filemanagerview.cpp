#include "filemanagerview.h"

#include "filemanagerwidget.h"

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IView(parent)
{
    m_widget = new FileManagerWidget();
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
