#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H

#include "guisystem_global.h"

#include <QtCore/QObject>

class QMenu;
class QMenuBar;
class QToolBar;

namespace GuiSystem {

class Command;
class CommandContainerPrivate;
class GUISYSTEM_EXPORT CommandContainer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandContainer)
    Q_DISABLE_COPY(CommandContainer)

    Q_PROPERTY(QByteArray id READ id)

public:
    explicit CommandContainer(const QByteArray &id, QObject *parent = 0);
    ~CommandContainer();

    void addCommand(Command *command, const QByteArray &group = QByteArray());
    void addContainer(CommandContainer *container, const QByteArray &group = QByteArray());
    void addGroup(const QByteArray &id);

    void clear();

    QList<Command*> commands(const QByteArray &id) const;

    QByteArray id() const;

    QMenu *menu() const;
    QMenuBar *menuBar() const;
    QToolBar *toolBar() const;

    QString title() const;
    void setTitle(const QString &title);

protected:
    CommandContainerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // COMMANDCONTAINER_H
