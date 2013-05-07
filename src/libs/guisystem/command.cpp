#include "command.h"
#include "abstractcommand_p.h"

#include <QtGui/QAction>
#include <QtGui/QKeySequence>

#include "actionmanager.h"
#include "proxyaction.h"

namespace GuiSystem {

class CommandPrivate : public AbstractCommandPrivate
{
    Q_DECLARE_PUBLIC(Command)
public:
    CommandPrivate(Command *qq) : AbstractCommandPrivate(qq) {}

    ProxyAction *action;
    QAction *realAction;

    Command::Attributes attributes;

    QKeySequence defaultShortcut;
    bool isSeparator;
    QKeySequence shortcut;
    QString toolTip;

    void init();
    void update();
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
};

} // namespace GuiSystem

using namespace GuiSystem;

void CommandPrivate::init()
{
    Q_Q(Command);
    action = new ProxyAction(q);
    action->setEnabled(false);
    realAction = 0;
    isSeparator = false;
}

void CommandPrivate::update()
{
//    if (attributes & Command::AttributeHide)
//        action->setVisible((bool)realAction);
    if (attributes & Command::AttributeUpdateText && !realAction)
        action->setText(text);
    if (attributes & Command::AttributeUpdateIcon && !realAction)
        action->setIcon(icon);
    if (attributes & Command::AttributeUpdateShortcut && !realAction) {
        action->setShortcut(defaultShortcut);
    }

    if (!realAction)
        action->setEnabled((attributes & Command::AttributeUpdateEnabled));
}

void CommandPrivate::setText(const QString &text)
{
    Q_Q(Command);

    if (this->text == text)
        return;

    this->text = text;
    if (!realAction || !(attributes & Command::AttributeUpdateText))
        action->setText(text);

    emit q->changed();
}

void CommandPrivate::setIcon(const QIcon &icon)
{
    Q_Q(Command);
    this->icon = icon;
    if (!realAction || !(attributes & Command::AttributeUpdateIcon))
        action->setIcon(icon);

    emit q->changed();
}

/*!
    \class GuiSystem::Command

    \brief The Command class allows to link QActions with entries in application's menu.

    This class links to actions added to widgets (using QWidget::addAction method) by id.
    Each action with objectName equal to Command's id is linked with it - when action's widet is visible,
    Command becomes enabled and vice versa. When user triggers Command's action in menu, linked action is
    triggered too.
*/

Command::Command(const QByteArray &id,
                 const QIcon &icon,
                 const QKeySequence &key,
                 const QString &text,
                 QObject *parent) :
    AbstractCommand(*new CommandPrivate(this), id, parent)
{
    Q_D(Command);
    d->init();

    setIcon(icon);
    setDefaultShortcut(key);
    setText(text);

    ActionManager::instance()->registerCommand(this);
}

Command::Command(const QByteArray &id,
                 const QKeySequence &key,
                 const QString &text,
                 QObject *parent) :
    AbstractCommand(*new CommandPrivate(this), id, parent)
{
    Q_D(Command);
    d->init();

    setDefaultShortcut(key);
    setText(text);

    ActionManager::instance()->registerCommand(this);
}

Command::Command(const QByteArray &id,
                 const QString &text,
                 QObject *parent) :
    AbstractCommand(*new CommandPrivate(this), id, parent)
{
    Q_D(Command);
    d->init();

    setText(text);

    ActionManager::instance()->registerCommand(this);
}

/*!
    \brief Constructs Command with \a id and register it in ActionManager.
*/
Command::Command(const QByteArray &id, QObject *parent) :
    AbstractCommand(*new CommandPrivate(this), id, parent)
{
    Q_D(Command);
    d->init();

    ActionManager::instance()->registerCommand(this);
}

/*!
    \brief Destroys and unregisters Command.
*/
Command::~Command()
{
    ActionManager::instance()->unregisterCommand(this);
}

/*!
    \enum GuiSystem::Command::Attribute

    This enum type describes Commands parameters.

    \value AttributeHide Command is hidden instead of disabling when no action is linked with it
    \value AttributeUpdateText Command uses linked action's text instead of defaultText
    \value AttributeUpdateIcon Command uses linked action's icon instead of defaultIcon
    \value AttributeNonConfigurable Command ignores linked aciton's parameters. This is default value.
*/

/*!
    \enum GuiSystem::Command::CommandContext

    This enum type set's Commands context, which can be one of following:

    \value WidgetCommand Command is linked to action added to focus widget.
    \value WidgetWithChildrenCommand Command is linked to action added to focus widget or one of it's parents.
    \value WindowCommand Command is linked to action added to widget that belongs to currently active window.
    \value ApplicationCommand Command is linked to it's own action and always enabled.
*/

/*!
    \brief Returns inner Command's action which is used in menus and menubars.
*/
QAction * Command::commandAction() const
{
    return d_func()->action;
}

/*!
    \property Command::attributes

    \brief Command's attributes.

    Default value is Command::AttributeNonConfigurable.
*/
Command::Attributes Command::attributes() const
{
    return d_func()->attributes;
}

bool Command::hasAttribute(Command::Attribute attr) const
{
    return d_func()->attributes & attr;
}

void Command::setAttributes(Attributes attrs)
{
    Q_D(Command);

    if (d->attributes != attrs) {
        d->attributes = attrs;

//        if (attrs & AttributeHide && !d->realAction)
//            d->action->setVisible(false);
        d->action->setAttributes(ProxyAction::Attributes((int)attrs >> 1));
        if (!d->realAction)
            d->action->setEnabled(attrs & AttributeUpdateEnabled);

        emit changed();
    }
}

/*!
    \property Command::defaultShortcut

    \brief Default Command's shortcut

    It is used as a shortcut for this command if AttributeNonConfigurable is set, otherwised is used
    action's shortcut.
*/
QKeySequence Command::defaultShortcut() const
{
    return d_func()->defaultShortcut;
}

void Command::setDefaultShortcut(const QKeySequence &key)
{
    Q_D(Command);

    if (d->defaultShortcut != key) {
        if (d->shortcut == d->defaultShortcut)
            d->shortcut = key;

        d->defaultShortcut = key;
        if (!d->realAction || !(d->attributes & AttributeUpdateShortcut))
            d->action->setShortcut(key);

        emit changed();
    }
}

bool Command::isSeparator() const
{
    Q_D(const Command);

    return d->isSeparator;
}

void Command::setSeparator(bool b)
{
    Q_D(Command);

    if (d->isSeparator == b)
        return;

    d->isSeparator = b;
    if (b)
        setRealAction(0);
    d->action->setSeparator(b);
}

/*!
    \property Command::shortcut

    \brief Overrided Command's shortcut

    Use this property to override Command's shortcut, for example from user preferences.
    Note, if shortcut is not equal to default shortcut, Command::AttributeUpdateShortcut is ignored
    and command always uses this shortcut.
*/
QKeySequence Command::shortcut() const
{
    return d_func()->shortcut;
}

void Command::setShortcut(const QKeySequence &key)
{
    Q_D(Command);

    if (d->shortcut == key)
        return;

    d->shortcut = key;

    if (d->shortcut != d->defaultShortcut) {
        d->action->setShortcut(key);
        d->action->setAttributes(ProxyAction::Attributes((int)(d->attributes & ~AttributeUpdateShortcut)));
    } else {
        if (d->attributes & AttributeUpdateShortcut && d->realAction)
            d->action->setShortcut(d->realAction->shortcut());
        else
            d->action->setShortcut(d->defaultShortcut);
        d->action->setAttributes(ProxyAction::Attributes((int)(d->attributes)));
    }
}

QString Command::toolTip() const
{
    Q_D(const Command);
    return d->toolTip;
}

void Command::setToolTip(const QString &toolTip)
{
    Q_D(Command);

    if (d->toolTip == toolTip)
        return;

    d->toolTip = toolTip;
    if (!d->realAction || !(d->attributes & Command::AttributeUpdateText))
        d->action->setToolTip(toolTip);

    emit changed();
}

/*!
    \property Command::data

    \brief Inner Command's data

    This property can be used to store user-specific data.
*/
QVariant Command::data() const
{
    return d_func()->action->data();
}

void Command::setData(const QVariant & data)
{
    d_func()->action->setData(data);
}

QAction *Command::realAction() const
{
    Q_D(const Command);

    return d->realAction;
}

/*!
    \internal

    Links \a action to this Command.
*/
void Command::setRealAction(QAction *action)
{
    Q_D(Command);

    if (d->realAction != action && !d->isSeparator) {
        d->realAction = action;
        d->action->setAction(action);
        if (shortcut() != defaultShortcut())
            d->action->setShortcut(shortcut());
        d->update();
    }
}

QAction * Command::createAction(QObject * /*parent*/) const
{
    Q_D(const Command);
    return d->action;
}

/*!
    \class GuiSystem::Separator

    \brief Helper class to add separators to CommandContainers.

    Due to plugin architecture, it is not possible to add separators that have containter as a parent,
    so we need this class to correctly remove separators when plugins are unloaded.
*/

/*!
    \brief Constructs Separator with the givena \a parent and registers it in ActionManager.

    Separator has auto-generated id that is calculated as a string representation of an object pointer.
*/
Separator::Separator(QObject *parent) :
    Command(QString::number(quintptr(this), 16).toLatin1(), parent)
{
    setAttributes(Command::AttributeNonConfigurable);
    setSeparator(true);
}
