#include "filemanagerview.h"

#include "dualpanewidget.h"

#include <QtGui/QAction>
#include <actionmanager.h>
#include <command.h>
#include <constants.h>

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IMainView(parent)
{
    m_widget = new DualPaneWidget();
//    m_widget->setDualPaneModeEnabled(true);
    m_widget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(pathChanged(QString)));

    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();
    actionManager->command(Constants::Ids::Actions::Open)->action(m_widget, SLOT(open()));
    actionManager->command(Constants::Ids::Actions::NewFolder)->action(m_widget, SLOT(newFolder()));
    actionManager->command(Constants::Ids::Actions::Remove)->action(m_widget, SLOT(remove()));

    QAction *a = actionManager->command(Constants::Ids::Actions::DualPane)->action();
    a->setCheckable(true);
    m_widget->addAction(a);
    connect(a, SIGNAL(toggled(bool)), m_widget, SLOT(setDualPaneModeEnabled(bool)));

    actionManager->command(Constants::Ids::Actions::Up)->action(m_widget, SLOT(up()));

    actionManager->command(Constants::Ids::Actions::Cut)->action(m_widget, SLOT(cut()));
    actionManager->command(Constants::Ids::Actions::Copy)->action(m_widget, SLOT(copy()));
    actionManager->command(Constants::Ids::Actions::Paste)->action(m_widget, SLOT(paste()));
    actionManager->command(Constants::Ids::Actions::SelectAll)->action(m_widget, SLOT(selectAll()));
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
