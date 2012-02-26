#include "command.h"

#include <QtGui/QAction>
#include <QtGui/QKeySequence>

#include "actionmanager.h"
#include "proxyaction.h"

namespace GuiSystem {

class CommandPrivate
{
public:
    CommandPrivate(const QByteArray &id, Command *qq);

    ProxyAction *action;
    QAction *realAction;

    Command::Attributes attributes;
    Command::CommandContext context;

    QKeySequence defaultShortcut;
    QIcon defaultIcon;
    QString defaultText;
    bool isSeparator;
    QKeySequence shortcut;

    QByteArray id;

    void update();
};

} // namespace GuiSystem

using namespace GuiSystem;

CommandPrivate::CommandPrivate(const QByteArray &id, Command *qq)
{
    this->id = id;
    action = new ProxyAction(qq);
    action->setEnabled(false);
    realAction = 0;
    isSeparator = false;
    context = Command::WidgetCommand;
}

void CommandPrivate::update()
{
//    if (attributes & Command::AttributeHide)
//        action->setVisible((bool)realAction);
    if (attributes & Command::AttributeUpdateText && !realAction)
        action->setText(defaultText);
    if (attributes & Command::AttributeUpdateIcon && !realAction)
        action->setIcon(defaultIcon);
    if (attributes & Command::AttributeUpdateShortcut && !realAction) {
        action->setShortcut(defaultShortcut);
    }
}

/*!
    \class Command

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
    QObject(parent),
    d_ptr(new CommandPrivate(id, this))
{
    setDefaultIcon(icon);
    setDefaultShortcut(key);
    setDefaultText(text);

    ActionManager::instance()->registerCommand(this);
}

Command::Command(const QByteArray &id,
                 const QKeySequence &key,
                 const QString &text,
                 QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate(id, this))
{
    setDefaultShortcut(key);
    setDefaultText(text);

    ActionManager::instance()->registerCommand(this);
}

Command::Command(const QByteArray &id,
                 const QString &text,
                 QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate(id, this))
{
    setDefaultText(text);

    ActionManager::instance()->registerCommand(this);
}

/*!
    \brief Constructs Command with \a id and register it in ActionManager.
*/
Command::Command(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate(id, this))
{
    ActionManager::instance()->registerCommand(this);
}

/*!
    \brief Destroys and unregisters Command.
*/
Command::~Command()
{
    ActionManager::instance()->unregisterCommand(this);

    delete d_ptr;
}

/*!
    \enum Command::Attribute

    This enum type describes Commands parameters.

    \value AttributeHide Command is hidden instead of disabling when no action is linked with it
    \value AttributeUpdateText Command uses linked action's text instead of defaultText
    \value AttributeUpdateIcon Command uses linked action's icon instead of defaultIcon
    \value AttributeNonConfigurable Command ignores linked aciton's parameters. This is default value.
*/

/*!
    \enum Command::CommandContext

    This enum type set's Commands context, which can be one of following:

    \value WidgetCommand Command is linked to action added to focus widget.
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

        emit changed();
    }
}

/*!
    \property Command::context

    \brief Command's context

    Default value is Command::WidgetCommand.
*/
Command::CommandContext Command::context() const
{
    return d_func()->context;
}

void Command::setContext(Command::CommandContext context)
{
    Q_D(Command);
    if (d->context == ApplicationCommand) {
        d->action->setEnabled(false);
    }

    if (context == ApplicationCommand) {
        d->action->setEnabled(true);
    }

    d->context = context;
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

/*!
    \property Command::defaultIcon

    \brief Default Command's icon

    It is used as an icon for this command if Command::AttributeUpdateIcon is not set, otherwised is used
    action's icon.
*/
QIcon Command::defaultIcon() const
{
    return d_func()->defaultIcon;
}

void Command::setDefaultIcon(const QIcon &icon)
{
    Q_D(Command);

    d->defaultIcon = icon;
    if (!d->realAction || !(d->attributes & AttributeUpdateIcon))
        d->action->setIcon(icon);

    emit changed();
}

/*!
    \property Command::defaultText

    \brief Default Command's text

    It is used as a text for this command if Command::AttributeUpdateText is not set, otherwised is used
    action's text.
*/
QString Command::defaultText() const
{
    return d_func()->defaultText;
}

void Command::setDefaultText(const QString &text)
{
    Q_D(Command);

    if (d->defaultText != text) {
        d->defaultText = text;
        if (!d->realAction || !(d->attributes & AttributeUpdateText))
            d->action->setText(text);

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

/*!
    \property Command::id

    \brief Command's id, which is used to identify commands.
*/
QByteArray Command::id() const
{
    return d_func()->id;
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
