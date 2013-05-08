#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include "../guisystem_global.h"

#include <QtCore/QObject>

#include "command.h"

class QAction;
class QMenu;

typedef QList<QAction*> QActionList;

namespace GuiSystem {

class CommandContainer;

class ActionManagerPrivate;
class GUISYSTEM_EXPORT ActionManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionManager)

public:
    explicit ActionManager(QObject *parent = 0);
    ~ActionManager();

    Command *command(const QByteArray &id);
    QList<Command *> commands() const;
    CommandContainer *container(const QByteArray &id);
    QList<CommandContainer *> containers() const;

    static ActionManager *instance();

    bool exportShortcuts(QIODevice *device) const;
    bool importShortcuts(QIODevice *device);

protected:
    void registerCommand(Command *cmd);
    void registerContainer(CommandContainer *c);

    void unregisterCommand(Command *cmd);
    void unregisterContainer(CommandContainer *c);

    bool eventFilter(QObject *o, QEvent *e);

protected:
    void setActionsEnabled(QWidget *w, bool enable, Qt::ShortcutContext context);

protected slots:
    void onFocusChanged(QWidget*,QWidget*);

protected:
    ActionManagerPrivate *d_ptr;

    friend class Command;
    friend class CommandContainer;
};

} // namespace GuiSystem

#endif // ACTIONMANAGER_H
