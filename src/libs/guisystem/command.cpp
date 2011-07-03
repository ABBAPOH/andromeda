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

    QKeySequence defaultShortcut;
    QIcon defaultIcon;
    QString defaultText;

    QString id;
};

} // namespace GuiSystem

using namespace GuiSystem;

Command::Command(const QString &id, QObject *parent) :
    QObject(parent),
    d_ptr(new CommandPrivate)
{
    Q_D(Command);

    d->id = id;
    d->action = new QAction(this);
    d->action->setEnabled(false);
    d->realAction = 0;

    connect(d->action, SIGNAL(triggered(bool)), SLOT(onTrigger(bool)));

    ActionManager::instance()->registerCommand(this);

    setAttributes(AttributeNonConfigurable);
}

Command::~Command()
{
    ActionManager::instance()->unregisterCommand(this);

    delete d_ptr;
}

QAction * Command::action() const
{
    return d_func()->action;
}

Command::Attributes Command::attributes() const
{
    return d_func()->attributes;
}

void Command::setAttributes(Attributes attr)
{
    d_func()->attributes = attr;
}

QKeySequence Command::defaultShortcut() const
{
    return d_func()->defaultShortcut;
}

void Command::setDefaultShortcut(const QKeySequence &key)
{
    Q_D(Command);

    d->defaultShortcut = key;
    if (!d->realAction)
        d->action->setShortcut(key);
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
}

QString Command::defaultText() const
{
    return d_func()->defaultText;
}

void Command::setDefaultText(const QString &text)
{
    Q_D(Command);

    d->defaultText = text;
    if (!d->realAction)
        d->action->setText(text);
}

QString Command::id() const
{
    return d_func()->id;
}

void Command::onTrigger(bool checked)
{
    Q_D(Command);

    if (d->realAction) {
        if (d->realAction->isCheckable())
            d->realAction->setChecked(checked);
        else
            d->realAction->trigger();
    }
}

void Command::setRealAction(QAction *action)
{
    Q_D(Command);

    d->realAction = action;
    if (d->attributes & AttributeHide)
        d->action->setVisible((bool)action);
    if (d->attributes & AttributeUpdateText)
        d->action->setText(action ? action->text() : d->defaultText);
    if (d->attributes & AttributeUpdateIcon)
        d->action->setIcon(action ? action->icon() : d->defaultIcon);
    if (!(d->attributes & AttributeNonConfigurable)) {
        d->action->setShortcut(action ? action->shortcut() : d->defaultShortcut);
        bool checkable = action ? action->isCheckable() : false;
        d->action->setCheckable(checkable);
        if (checkable)
            d->action->setChecked(action->isChecked());
    }

    d->action->setEnabled(action ? action->isEnabled() : false);
}
