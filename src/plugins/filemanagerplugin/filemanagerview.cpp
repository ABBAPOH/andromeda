#include "filemanagerview.h"

#include "dualpanewidget.h"

#include <QtGui/QAction>
#include <QtCore/QSettings>
#include <actionmanager.h>
#include <command.h>
#include <constants.h>

using namespace FileManagerPlugin;

FileManagerView::FileManagerView(QObject *parent) :
    IMainView(parent)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    bool enableDualPane = settings.value("dualPaneModeEnabled").toBool();
    settings.endGroup();

    m_widget = new DualPaneWidget();
    m_widget->setDualPaneModeEnabled(enableDualPane);
    m_widget->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(m_widget, SIGNAL(currentPathChanged(QString)), SIGNAL(pathChanged(QString)));
    connect(m_widget, SIGNAL(openRequested(QString)), SIGNAL(openRequested(QString)));

    GuiSystem::ActionManager *actionManager = GuiSystem::ActionManager::instance();
    actionManager->command(Constants::Ids::Actions::Open)->action(m_widget, SLOT(open()));
    actionManager->command(Constants::Ids::Actions::NewFolder)->action(m_widget, SLOT(newFolder()));
    actionManager->command(Constants::Ids::Actions::Remove)->action(m_widget, SLOT(remove()));

    QAction *a = actionManager->command(Constants::Ids::Actions::DualPane)->action();
    a->setCheckable(true);
    a->setChecked(enableDualPane);
    m_widget->addAction(a);
    connect(a, SIGNAL(toggled(bool)), this, SLOT(setDualPaneModeEnabled(bool)));

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

void FileManagerView::setDualPaneModeEnabled(bool on)
{
    QSettings settings;
    settings.beginGroup("FileManager");
    settings.setValue("dualPaneModeEnabled", on);
    settings.endGroup();
    m_widget->setDualPaneModeEnabled(on);
}

QString FileManagerFactory::id() const
{
    return "FileManager";
}

GuiSystem::IView * FileManagerPlugin::FileManagerFactory::createView() const
{
    return new FileManagerView;
}
