#include "filemanagerview.h"

#include "filemanagerwidget.h"

#include <state.h>

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IMainView(parent)
{
    m_widget = new FileManagerWidget();
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(pathChanged(QString)));
}

void FileManagerView::initialize()
{
}

QString FileManagerView::type() const
{
    return "filemanager";
}

QWidget * FileManagerView::widget() const
{
    return m_widget;
}

bool FileManagerView::open(const QString &path)
{
    m_widget->setCurrentPath(path);
    return true;
}

bool FileManagerView::open(const HistoryItem &item)
{
    m_widget->history()->goToItem(item);
    return true;
}

HistoryItem FileManagerView::currentItem() const
{
    return m_widget->history()->currentItem();
}

QString FileManagerFactory::id() const
{
    return "FileManager";
}

GuiSystem::IView * FileManagerPlugin::FileManagerFactory::createView() const
{
    return new FileManagerView;
}
