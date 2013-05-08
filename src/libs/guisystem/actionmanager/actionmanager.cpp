#include "actionmanager.h"

#include "command.h"
#include "commandcontainer.h"

#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QHash>
#include <QtCore/QSettings>
#include <QtCore/QXmlStreamWriter>

#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QMenu>

// TODO: update containers correctly when adding new actions

namespace GuiSystem {

class ActionManagerPrivate
{
public:
    ActionManagerPrivate() {}

    QHash<QByteArray, AbstractCommand *> commands;
    QSettings *settings;
};

} // namespace GuiSystem

using namespace GuiSystem;

/*!
    \class GuiSystem::ActionManager

    \brief The ActionManager class handles Commands and CommandContainers

    This class allows to implicitly register Commands and CommandContainers and recieve them by their id.
    Commands and CommandContainers are registered when created using their constructor. Once registered,
    they can by retreived using ActionManager::command and ActionManager::container methods.
*/

/*!
    Creates a ActionManager with the given \a parent. You are not suppose to construct ActionManager
    manually, use ActionManager::instance instead.
*/
ActionManager::ActionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new ActionManagerPrivate)
{
    Q_D(ActionManager);
    d->settings = new QSettings(this);
    d->settings->beginGroup(QLatin1String("ActionManager/Shortcuts"));

    qApp->installEventFilter(this);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), SLOT(onFocusChanged(QWidget*,QWidget*)));
}

/*!
    \brief Destructor
*/
ActionManager::~ActionManager()
{
    Q_D(ActionManager);

    foreach (AbstractCommand *o, d->commands.values()) {
        if (o->parent() == this)
            delete o;
    }

    delete d_ptr;
}

/*!
    Returns pointer to Command previously created with \id
*/
Command * ActionManager::command(const QByteArray &id)
{
    Q_D(ActionManager);

    Command *c = qobject_cast<Command *>(d->commands.value(id));
    Q_ASSERT_X(c,
               "ActionManager::command",
               QString("Can't find command with id %1").arg(QString::fromUtf8(id)).toUtf8());
    if (!c)
        qWarning() << "ActionManager::command :" << "Can't find command with id" << id;

    return c;
}

/*!
    \brief Returns list of all Commands registered in ActionManager.
*/
QList<Command *> ActionManager::commands() const
{
    Q_D(const ActionManager);

    QList<Command *> result;
    foreach (AbstractCommand *object, d->commands) {
        Command *c = qobject_cast<Command *>(object);
        if (c)
            result.append(c);
    }
    return result;
}

/*!
    Returns pointer to CommandContainer previously created with \id
*/
CommandContainer * ActionManager::container(const QByteArray &id)
{
    Q_D(ActionManager);

    CommandContainer *c = qobject_cast<CommandContainer *>(d->commands.value(id));
    Q_ASSERT_X(c,
               "ActionManager::container",
               QString("Can't find container with id %1").arg(QString::fromUtf8(id)).toUtf8());
    if (!c)
        qWarning() << "ActionManager::container :" << "Can't find command with id" << id;

    return c;
}

/*!
    \brief Returns list of all Containers registered in ActionManager.
*/
QList<CommandContainer *> ActionManager::containers() const
{
    Q_D(const ActionManager);

    QList<CommandContainer *> result;
    foreach (QObject *object, d->commands) {
        CommandContainer *c = qobject_cast<CommandContainer *>(object);
        if (c)
            result.append(c);
    }
    return result;

}

Q_GLOBAL_STATIC(ActionManager, get_instance)
/*!
    Returns pointer to static instance of an ActionManager.
    Ths method should only be called after QApplication is constructed.
*/
ActionManager *ActionManager::instance()
{
    return get_instance();
}

bool ActionManager::exportShortcuts(QIODevice *device) const
{
    if (device->openMode() != QIODevice::WriteOnly)
        return false;

    QXmlStreamWriter w(device);
    w.setAutoFormatting(true);
    w.setAutoFormattingIndent(1); // Historical, used to be QDom.
    w.writeStartDocument();
    w.writeDTD(QLatin1String("<!DOCTYPE KeyboardMappingScheme>"));
    w.writeComment(QString::fromAscii(" Written by Andromeda at %1. ").
                   arg(QDateTime::currentDateTime().toString(Qt::ISODate)));
    w.writeStartElement(QLatin1String("mapping"));
    foreach (Command *c, commands()) {
        const QByteArray id = c->id();
        QKeySequence shortcut = c->shortcut();
        if (shortcut.isEmpty()) {
            w.writeEmptyElement(QLatin1String("shortcut"));
            w.writeAttribute(QLatin1String("id"), id);
        } else {
            w.writeStartElement(QLatin1String("shortcut"));
            w.writeAttribute(QLatin1String("id"), id);
            w.writeEmptyElement(QLatin1String("key"));
            w.writeAttribute(QLatin1String("value"), shortcut.toString());
            w.writeEndElement(); // Shortcut
        }
    }
    w.writeEndElement();
    w.writeEndDocument();

    return true;
}

bool ActionManager::importShortcuts(QIODevice *device)
{
    Q_D(ActionManager);

    if (device->openMode() != QIODevice::ReadOnly)
        return false;

    QXmlStreamReader r(device);

    QString currentId;

    while (!r.atEnd()) {
        switch (r.readNext()) {
        case QXmlStreamReader::StartElement: {
            const QStringRef name = r.name();
            if (name == QLatin1String("shortcut")) {
                currentId = r.attributes().value(QLatin1String("id")).toString();
            } else if (name == QLatin1String("key")) {
                if (currentId.isEmpty())
                    return false;

                const QXmlStreamAttributes attributes = r.attributes();
                Command *c = command(currentId.toUtf8());
                if (attributes.hasAttribute(QLatin1String("value"))) {
                    const QString keyString = attributes.value(QLatin1String("value")).toString();
                    if (c) {
                        c->setShortcut(QKeySequence(keyString));
                        d->settings->setValue(c->id(), c->shortcut().toString(QKeySequence::NativeText));
                    }
                } else {
                    if (c) {
                        c->setShortcut(QKeySequence());
                        d->settings->setValue(c->id(), QString());
                    }
                }
                currentId.clear();
            } // if key element
        } // case QXmlStreamReader::StartElement
        default:
            break;
        } // switch
    } // while !atEnd

    if (r.hasError())
        return false;

    return true;
}

/*!
    \internal
*/
void ActionManager::registerCommand(Command *cmd)
{
    Q_D(ActionManager);

    Q_ASSERT(!d->commands.contains(cmd->id()));

    d->commands.insert(cmd->id(), cmd);
    if (!cmd->parent())
        cmd->setParent(this);

    cmd->setShortcut(d->settings->value(cmd->id(), cmd->defaultShortcut().toString(QKeySequence::NativeText)).toString());
}

/*!
    \internal
*/
void ActionManager::registerContainer(CommandContainer *c)
{
    Q_D(ActionManager);

    Q_ASSERT(!d->commands.contains(c->id()));

    d->commands.insert(c->id(), c);
    if (!c->parent())
        c->setParent(this);
}

/*!
    \internal
*/
void ActionManager::unregisterCommand(Command *cmd)
{
    Q_D(ActionManager);

    d->commands.remove(cmd->id());
    if (cmd->parent() == this)
        cmd->deleteLater();
}

/*!
    \internal
*/
void ActionManager::unregisterContainer(CommandContainer *c)
{
    Q_D(ActionManager);

    d->commands.remove(c->id());
    if (c->parent() == this)
        c->deleteLater();
}

/*!
    \internal

    Monitors changing of widgets in application to keep Commands up to date with visible widgets.
*/
bool ActionManager::eventFilter(QObject *o, QEvent *e)
{
    if (o->isWidgetType()) {
        QWidget *w = static_cast<QWidget*>(o);

        if (e->type() == QEvent::Show) {
            if (w->isActiveWindow()) {
                setActionsEnabled(w, true, Qt::WindowShortcut);
            }
        } else if (e->type() == QEvent::Hide) {
            setActionsEnabled(w, false, Qt::WindowShortcut);
        } else if (e->type() == QEvent::ActivationChange) {
            bool enable = w->isActiveWindow();
            QWidgetList widgets = w->findChildren<QWidget*>();
            widgets.prepend(w);
            foreach (QWidget *w, widgets) {
                if (w->isVisible())
                    setActionsEnabled(w, enable, Qt::WindowShortcut);
            }
        }
    }
    return QObject::eventFilter(o, e);
}

void ActionManager::onFocusChanged(QWidget *old, QWidget *newWidget)
{
    // TODO: check this carefully
    if (qobject_cast<QMenu*>(newWidget))
        return;

    QWidget *w = old;
    if (w)
        setActionsEnabled(w, false, Qt::WidgetShortcut);
    while (w) {
        setActionsEnabled(w, false, Qt::WidgetWithChildrenShortcut);
        w = w->parentWidget();
    }

    w = newWidget;
    if (w)
        setActionsEnabled(w, true, Qt::WidgetShortcut);
    while (w) {
        setActionsEnabled(w, true, Qt::WidgetWithChildrenShortcut);
        w = w->parentWidget();
    }
}

/*!
    \internal

    Connects actions for widget \w to Commands with id equal to actions' objectNames.
*/
void ActionManager::setActionsEnabled(QWidget *w, bool enabled, Qt::ShortcutContext context)
{
    Q_D(ActionManager);

    foreach (QAction *action, w->actions()) {
        QByteArray id = action->objectName().toUtf8();
        if (!id.isEmpty()) {
            Command *c = qobject_cast<Command *>(d->commands.value(id));
            if (c && action->shortcutContext() == context) {
                if (enabled) {
                    c->setRealAction(action);
                } else {
                    if (c->realAction() == action)
                        c->setRealAction(0);
                }
            }
        }
    }
}
