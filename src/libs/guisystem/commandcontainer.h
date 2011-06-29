#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H

#include <QtCore/QObject>

class QMenu;
class QMenuBar;

namespace GuiSystem {

class Command;
class CommandContainerPrivate;
class CommandContainer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CommandContainer)
    Q_DISABLE_COPY(CommandContainer)
public:
    explicit CommandContainer(const QString &id, QObject *parent = 0);
    ~CommandContainer();

    void addCommand(Command *command);
    void addContainer(CommandContainer *container);

    QMenu *menu() const;
    QMenuBar *menuBar() const;

    void clear();

signals:

public slots:

protected:
    CommandContainerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // COMMANDCONTAINER_H
