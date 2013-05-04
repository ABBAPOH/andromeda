#include "action.h"

#include "actionmanager.h"

using namespace GuiSystem;

/*!
    \class GuiSystem::Action

    \brief The Action class is convenience class to make registering actions easier
*/

/*!
    Creates an Action with the given \a parent and registers it in ActionManager
    with \a id.
*/
Action::Action(const QByteArray &id, QObject *parent) :
    QAction(parent)
{
    ActionManager *am = ActionManager::instance();
    am->registerAction(this, id);

    Command *c = am->command(id);
    if (c) {
        setText(c->text());
        setIcon(c->icon());
    }
}

/*!
    Creates an Action with the given \a parent and \a text and registers it in
    ActionManager with \a id.
*/
Action::Action(const QString &text, const QByteArray &id, QObject *parent) :
    QAction(text, parent)
{
    ActionManager *am = ActionManager::instance();
    am->registerAction(this, id);

    Command *c = am->command(id);
    if (c) {
        setIcon(c->icon());
    }
}

/*!
    Creates an Action with the given \a parent, \a text and \a icon and registers
    it in ActionManager with \a id.
*/
Action::Action(const QIcon &icon, const QString &text, const QByteArray &id, QObject *parent) :
    QAction(icon, text, parent)
{
    ActionManager *am = ActionManager::instance();
    am->registerAction(this, id);
}
