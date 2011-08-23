#include "command.h"

#include <QtGui/QAction>
#include <QtGui/QKeySequence>

#include "actionmanager.h"

namespace GuiSystem {

class CommandPrivate
{
public:
    QAction *action;
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
    if (attributes & Command::AttributeHide)
        action->setVisible((bool)realAction);
    if (attributes & Command::AttributeUpdateText)
        action->setText(realAction ? realAction->text() : defaultText);
    if (attributes & Command::AttributeUpdateIcon)
        action->setIcon(realAction ? realAction->icon() : defaultIcon);
    if (!(attributes & Command::AttributeNonConfigurable)) {
        action->setShortcut(realAction ? realAction->shortcut() : defaultShortcut);
    }

    bool checkable = realAction ? realAction->isCheckable() : false;
    action->setCheckable(checkable);
    if (checkable) {
        action->blockSignals(true); // prevent from throwing signal on focus change
        action->setChecked(realAction->isChecked());
        action->blockSignals(false);
    }
    action->setEnabled(alwaysEnabled || (realAction ? realAction->isEnabled() : false));
}

Command::Command(const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate)
{
    Q_D(Command);

    d->id = id;
    d->action = new QAction(this);
    d->action->setEnabled(false);
    d->realAction = 0;
    d->alwaysEnabled = false;
    d->context = WidgetCommand;

    connect(d->action, SIGNAL(triggered(bool)), SLOT(onTrigger(bool)));
    connect(d->action, SIGNAL(toggled(bool)), SLOT(onToggle(bool)));

    ActionManager::instance()->registerCommand(this);

    setAttributes(AttributeNonConfigurable);
}

Command::~Command()
{
    ActionManager::instance()->unregisterCommand(this);

    delete d_ptr;
}

/*!
    \brief Creates new action with objectName equal to command's id.

    Also command sets action's text, icon and shortcut default values (as passed to setDefault* methods).
    This action can be used in widgets and will be automatically triggered when widget have focus.
*/
QAction * Command::action()
{
    Q_D(Command);

    if (d->context == ApplicationCommand) {
        return d->realAction;
    }

    QAction *a = new QAction(this);
    a->setObjectName(id());
    a->setIcon(defaultIcon());
    a->setShortcut(defaultShortcut());
    a->setShortcutContext(Qt::WidgetShortcut);
    a->setText(defaultText());
    a->setData(data());
    a->setCheckable(d_func()->action->isCheckable());
    return a;
}

QAction * Command::action(QWidget *w, const char *slot)
{
    QAction * a = action();
    w->addAction(a);
    connect(a, SIGNAL(triggered()), w, slot);
    return a;
}

/*!
    \brief Returns inner Command's action which is used in menus and menubars.
*/
QAction * Command::commandAction() const
{
    return d_func()->action;
}

Command::Attributes Command::attributes() const
{
    return d_func()->attributes;
}

void Command::setAttributes(Attributes attrs)
{
    Q_D(Command);

    if (d->attributes != attrs) {
        d->attributes = attrs;

        emit changed();
    }
}

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
        d->realAction = action();
        d->action->setEnabled(true);
    }

    d->context = context;
}

QKeySequence Command::defaultShortcut() const
{
    return d_func()->defaultShortcut;
}

void Command::setDefaultShortcut(const QKeySequence &key)
{
    Q_D(Command);

    if (d->defaultShortcut != key) {
        d->defaultShortcut = key;
        if (!d->realAction)
            d->action->setShortcut(key);

        emit changed();
    }
}

QIcon Command::defaultIcon() const
{
    return d_func()->defaultIcon;
}

void Command::setDefaultIcon(const QIcon &icon)
{
    Q_D(Command);

    d->defaultIcon = icon;
    if (!d->realAction)
        d->action->setIcon(icon);

    emit changed();
}

QString Command::defaultText() const
{
    return d_func()->defaultText;
}

void Command::setDefaultText(const QString &text)
{
    Q_D(Command);

    if (d->defaultText != text) {
        d->defaultText = text;
        if (!d->realAction)
            d->action->setText(text);

        emit changed();
    }
}

bool Command::isSeparator() const
{
    return d_func()->action->isSeparator();
}

void Command::setSeparator(bool b)
{
    d_func()->action->setSeparator(b);
}

QVariant Command::data() const
{
    return d_func()->action->data();
}

void Command::setData(const QVariant & data)
{
    d_func()->action->setData(data);
}

/*!
    \brief Returns id of this command.

    When widget that have focus (or one of this parents) has action with objectName equal to id,
    actionmanager enables this command in all menus. When command action is triggered, Command
    automatically triggers action in focus widget.
*/
QByteArray Command::id() const
{
    return d_func()->id;
}

void Command::onTrigger(bool checked)
{
    Q_D(Command);

    if (d->realAction && !d->action->isCheckable()) {
        d->realAction->trigger();
    }
}

void Command::onToggle(bool checked)
{
    Q_D(Command);

    if (d->realAction && d->action->isCheckable()) {
        d->realAction->setChecked(checked);
    }
}

void Command::setRealAction(QAction *action)
{
    Q_D(Command);

    if (d->realAction != action) {
        d->realAction = action;
        d->update();
    }
}
