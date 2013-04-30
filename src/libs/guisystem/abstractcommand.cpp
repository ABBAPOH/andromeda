#include "abstractcommand.h"
#include "abstractcommand_p.h"

using namespace GuiSystem;

void AbstractCommandPrivate::setText(const QString &text)
{
    Q_Q(AbstractCommand);
    if (this->text == text)
        return;

    this->text = text;
    emit q->changed();
}

void AbstractCommandPrivate::setIcon(const QIcon &icon)
{
    Q_Q(AbstractCommand);

    this->icon = icon;
    emit q->changed();
}

/*!
    \class GuiSystem::AbstractCommand

    \brief Base class for Command and CommandContainer.

    Normally, you should not use this class directly, use its subclasses instead.
*/

/*!
    \internal
*/
AbstractCommand::AbstractCommand(AbstractCommandPrivate &dd, const QByteArray &id, QObject *parent) :
    QObject(parent),
    d_ptr(&dd)
{
    Q_D(AbstractCommand);
    d->id = id;
}

AbstractCommand::~AbstractCommand()
{
    delete d_ptr;
}

/*!
    \property AbstractCommand::id

    \brief  Id is used to identify commands.
*/
QByteArray AbstractCommand::id() const
{
    Q_D(const AbstractCommand);
    return d->id;
}

/*!
    \property AbstractCommand::text

    \brief Default command's text

    Text is displayed in command settings and in menus unless
    Command::AttributeUpdateText is specified.
*/
QString AbstractCommand::text() const
{
    Q_D(const AbstractCommand);
    return d->text;
}

void AbstractCommand::setText(const QString &text)
{
    Q_D(AbstractCommand);
    d->setText(text);
}

void AbstractCommand::setIcon(const QIcon &icon)
{
    Q_D(AbstractCommand);
    d->setIcon(icon);
}

/*!
    \property AbstractCommand::icon

    \brief Default command's icon

    Icon is displayed in command settings and in menus unless
    Command::AttributeUpdateIcon is specified.
*/
QIcon AbstractCommand::icon() const
{
    Q_D(const AbstractCommand);
    return d->icon;
}

