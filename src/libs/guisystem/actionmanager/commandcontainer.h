#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H

#include "../guisystem_global.h"

#include "abstractcommand.h"

class QMenu;
class QMenuBar;
class QToolBar;

namespace GuiSystem {

class Command;
class CommandContainerPrivate;
class GUISYSTEM_EXPORT CommandContainer : public AbstractCommand
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandContainer)
    Q_DISABLE_COPY(CommandContainer)

public:
    explicit CommandContainer(const QByteArray &id, QObject *parent = 0);
    ~CommandContainer();

    void addCommand(AbstractCommand *command, const QByteArray &weight = QByteArray());
    void addSeparator();

    void clear();

    QList<AbstractCommand *> commands() const;

    QMenu *menu(QWidget *parent = 0) const;
    QMenuBar *menuBar() const;
    QToolBar *toolBar(QWidget *parent = 0) const;

private slots:
    void onDestroy(QObject *);

protected:
    virtual QMenu *createMenu(QWidget *parent) const;
    virtual QToolBar *createToolBar(QWidget *parent) const;
    QAction *createAction(QObject *parent) const;
};

} // namespace GuiSystem

#endif // COMMANDCONTAINER_H
