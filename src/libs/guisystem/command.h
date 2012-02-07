#ifndef COMMAND_H
#define COMMAND_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

class QAction;
class QIcon;

namespace GuiSystem {

class CommandPrivate;
class GUISYSTEM_EXPORT Command : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Command)
    Q_DISABLE_COPY(Command)

    Q_PROPERTY(Attributes attributes READ attributes WRITE setAttributes)
    Q_PROPERTY(CommandContext context READ context WRITE setContext)
    Q_PROPERTY(QKeySequence defaultShortcut READ defaultShortcut WRITE setDefaultShortcut)
    Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
    Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
    Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)
    Q_PROPERTY(QVariant data READ data WRITE setData)
    Q_PROPERTY(QByteArray id READ id)

public:
    enum Attribute {
        AttributeNonConfigurable = 0,
        AttributeHide = 1,
        AttributeUpdateText = 2,
        AttributeUpdateIcon = 4,
        AttributeUpdateShortcut = 8
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)

    enum CommandContext {
        WidgetCommand,
        WindowCommand,
        ApplicationCommand
    };
    Q_ENUMS(CommandContext)

    Command(const QByteArray &id,
            const QIcon &icon,
            const QKeySequence &key,
            const QString &text,
            QObject *parent = 0);
    Command(const QByteArray &id,
            const QKeySequence &key,
            const QString &text,
            QObject *parent = 0);
    Command(const QByteArray &id,
            const QString &text,
            QObject *parent = 0);
    explicit Command(const QByteArray &id, QObject *parent = 0);
    ~Command();

    QAction *commandAction() const;

    Attributes attributes() const;
    bool hasAttribute(Attribute attr) const;
    void setAttributes(Attributes attr);

    CommandContext context() const;
    void setContext(CommandContext context);

    QKeySequence defaultShortcut() const;
    void setDefaultShortcut(const QKeySequence &key);

    QIcon defaultIcon() const;
    void setDefaultIcon(const QIcon &icon);

    QString defaultText() const;
    void setDefaultText(const QString &text);

    QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &key);

    QByteArray id() const;

    void setData(const QVariant &data);
    QVariant data() const;

signals:
    void changed();

protected:
    void setRealAction(QAction *commandAction);

protected:
    CommandPrivate *d_ptr;

    friend class ActionManager;
    friend class CommandContainer;
};

} // namespace GuiSystem

#endif // COMMAND_H
