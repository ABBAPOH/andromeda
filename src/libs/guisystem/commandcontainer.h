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

    void addCommand(Command *command, const QByteArray &weight = QByteArray());
    void addContainer(CommandContainer *container, const QByteArray &weight = QByteArray());

    void clear();

    QList<Command*> commands() const;
    QList<Command*> commands(const QByteArray &id) const;

    QByteArray id() const;

    QMenu *menu(QWidget *parent = 0) const;
    QMenuBar *menuBar() const;
    QToolBar *toolBar(QWidget *parent = 0) const;

    QString title() const;
    void setTitle(const QString &title);

private slots:
    void onDestroy(QObject *);

protected:
    virtual QMenu *createMenu(QWidget *parent) const;
    virtual QToolBar *createToolBar(QWidget *parent) const;

protected:
    CommandContainerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // COMMANDCONTAINER_H
