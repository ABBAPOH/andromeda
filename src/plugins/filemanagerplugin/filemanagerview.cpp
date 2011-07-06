#include "filemanagerview.h"

#include "filemanagerwidget.h"

#include <actionmanager.h>
#include <command.h>
#include <mainwindowplugin.h>

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IMainView(parent)
{
    m_widget = new FileManagerWidget();
    m_widget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(pathChanged(QString)));

    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();
    actionManager->command(MainWindowPlugin::ACTION_OPEN)->action(m_widget, SLOT(open()));
    actionManager->command(MainWindowPlugin::ACTION_NEW_FOLDER)->action(m_widget, SLOT(newFolder()));
    actionManager->command(MainWindowPlugin::ACTION_REMOVE)->action(m_widget, SLOT(remove()));

    actionManager->command(MainWindowPlugin::ACTION_BACK)->action(m_widget, SLOT(back()));
    actionManager->command(MainWindowPlugin::ACTION_FORWARD)->action(m_widget, SLOT(forward()));
    actionManager->command(MainWindowPlugin::ACTION_UP)->action(m_widget, SLOT(up()));
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
