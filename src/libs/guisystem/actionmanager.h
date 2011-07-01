#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QtCore/QObject>

class QAction;
class QMenu;

typedef QList<QAction*> QActionList;

namespace GuiSystem {

class Command;
class CommandContainer;

class ActionManagerPrivate;
class ActionManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionManager)
public:
    explicit ActionManager(QObject *parent = 0);
    ~ActionManager();

    void registerCommand(Command *cmd);
    void registerContainer(CommandContainer *c);

    Command *command(const QString &id);
    CommandContainer *container(const QString &id);

    static ActionManager *instance();

protected:
    ActionManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // ACTIONMANAGER_H
