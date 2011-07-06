#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "guisystem_global.h"

#include <QtCore/QObject>

class QAction;
class QMenu;

typedef QList<QAction*> QActionList;

namespace GuiSystem {

class Command;
class CommandContainer;

class ActionManagerPrivate;
class GUISYSTEM_EXPORT ActionManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionManager)
public:
    explicit ActionManager(QObject *parent = 0);
    ~ActionManager();

    Command *command(const QString &id);
    CommandContainer *container(const QString &id);

    static ActionManager *instance();

protected:
    void registerCommand(Command *cmd);
    void registerContainer(CommandContainer *c);

    void unregisterCommand(Command *cmd);
    void unregisterContainer(CommandContainer *c);

private slots:
    void onFocusChange(QWidget * old, QWidget * now);

protected:
    ActionManagerPrivate *d_ptr;

    friend class Command;
    friend class CommandContainer;
};

} // namespace GuiSystem

#endif // ACTIONMANAGER_H
