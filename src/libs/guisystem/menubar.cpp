#include "menubar.h"

#include "actionmanager.h"

using namespace GuiSystem;

MenuBar::MenuBar(QWidget *parent) :
    QMenuBar(parent)
{
    ActionManager *manager = ActionManager::instance();
    QList<QAction*> actions = manager->actions();
    for (int i = 0; i < actions.size(); i++) {
        qDebug("add action");
        addAction(actions[i]);
    }
    connect(manager, SIGNAL(actionAdded(QAction*)), SLOT(onActionAdd(QAction*)));
    qDebug("menubar");
}

void MenuBar::onActionAdd(QAction *action)
{
    addAction(action);
}
