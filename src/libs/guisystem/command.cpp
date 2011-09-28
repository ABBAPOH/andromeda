#include "command.h"

#include <QtGui/QAction>
#include <QtGui/QKeySequence>

#include "actionmanager.h"
#include "proxyaction.h"

namespace GuiSystem {

class CommandPrivate
{
public:
    ProxyAction *action;
    QAction *realAction;

    Command::Attributes attributes;
    Command::CommandContext context;

    bool alwaysEnabled;

    QKeySequence defaultShortcut;
    QIcon defaultIcon;
    QString defaultText;

    QByteArray id;

    void update();
};

} // namespace GuiSystem

using namespace GuiSystem;

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

/*!
    \brief Constructs Command with \a id and register it in ActionManager.
*/
Command::Command(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate)
{
    Q_D(Command);

    d->id = id;
    d->action = new ProxyAction(this);
    d->action->setEnabled(false);
    d->realAction = 0;
    d->alwaysEnabled = false;
    d->context = WidgetCommand;

    ActionManager::instance()->registerCommand(this);

    setAttributes(AttributeNonConfigurable);
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
    \brief Creates new action with objectName equal to command's id.

    Also command sets action's text, icon and shortcut default values (as passed to setDefault* methods).
    This action can be used in widgets and will be automatically triggered when Command is triggered.
*/
QAction * Command::action(QObject *parent)
{
    Q_D(Command);

    if (d->context == ApplicationCommand) {
        return d->realAction;
    }

    QAction *a = new QAction(parent ? parent : this);
    a->setObjectName(id());
    a->setIcon(defaultIcon());
    a->setShortcut(defaultShortcut());
    a->setShortcutContext(Qt::WidgetShortcut);
    a->setText(defaultText());
    a->setData(data());
    a->setCheckable(d_func()->action->isCheckable());
    return a;
}


/*!
    \brief Creates new action with objectName equal to command's id.

    This overload connects action's triggered() signal to slot \a slot
*/
QAction * Command::action(QObject *parent, const char *slot)
{
    QAction *a = action(parent);

    if (isCheckable())
        connect(a, SIGNAL(toggled(bool)), parent, slot);
    else
        connect(a, SIGNAL(triggered()), parent, slot);

    return a;
}

/*!
    \brief Creates new action with objectName equal to command's id.

    This overload adds newly created action to widget \a w and connects triggered() signal to slot \a slot
*/
QAction * Command::action(QWidget *widget, const char *slot)
{
    QAction *a = action((QObject *)widget, slot);
    widget->addAction(a);
    return a;
}

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

void Command::setAttributes(Attributes attrs)
{
    Q_D(Command);

    if (d->attributes != attrs) {
        d->attributes = attrs;

//        if (attrs & AttributeHide && !d->realAction)
//            d->action->setVisible(false);
        d->action->setAttributes(ProxyAction::Attributes((int)attrs));

        emit changed();
    }
}

/*!
    \property Command::isCheckable

    \brief Whether action in menu can be checked, or not.

    Default value is false.
*/
bool Command::isCheckable() const
{
    return d_func()->action->isCheckable();
}

void Command::setCheckable(bool b)
{
    Q_D(Command);

    if (d->action->isCheckable() != b) {
        d->action->setCheckable(b);

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
        delete d->realAction;
        d->action->setEnabled(false);
    }

    if (context == ApplicationCommand) {
        d->realAction = action(this);
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

    if (d->realAction != action) {
        d->realAction = action;
        d->action->setAction(action);
        d->update();
    }
}
