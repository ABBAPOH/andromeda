#ifndef COMMAND_H
#define COMMAND_H

#include "guisystem_global.h"

#include "abstractcommand.h"

#include <QtCore/QVariant>

#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

class QAction;
class QIcon;

namespace GuiSystem {

class CommandPrivate;
class GUISYSTEM_EXPORT Command : public AbstractCommand
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Command)
    Q_DISABLE_COPY(Command)

    Q_PROPERTY(Attributes attributes READ attributes WRITE setAttributes)
    Q_PROPERTY(QKeySequence defaultShortcut READ defaultShortcut WRITE setDefaultShortcut)
    Q_PROPERTY(bool isSeparator READ isSeparator WRITE setSeparator)
    Q_PROPERTY(QKeySequence shortcut READ shortcut WRITE setShortcut)
    Q_PROPERTY(QVariant data READ data WRITE setData)
    Q_PROPERTY(QString toolTip READ toolTip WRITE setToolTip)

public:
    enum Attribute {
        AttributeNonConfigurable = 0x1,
        AttributeHide = 0x2,
        AttributeUpdateText = 0x4,
        AttributeUpdateIcon = 0x8,
        AttributeUpdateShortcut = 0x10,
        AttributeUpdateEnabled = 0x20
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)

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

    QKeySequence defaultShortcut() const;
    void setDefaultShortcut(const QKeySequence &key);

    bool isSeparator() const;
    void setSeparator(bool b);

    QKeySequence shortcut() const;
    void setShortcut(const QKeySequence &key);

    QString toolTip() const;
    void setToolTip(const QString &toolTip);

    void setData(const QVariant &data);
    QVariant data() const;

protected:
    QAction *realAction() const;
    void setRealAction(QAction *commandAction);

    QAction *createAction(QObject *parent) const;

protected:
    friend class ActionManager;
    friend class CommandContainer;
};

class GUISYSTEM_EXPORT Separator : public Command
{
    Q_OBJECT
    Q_DISABLE_COPY(Separator)

public:
    explicit Separator(QObject *parent = 0);
};

} // namespace GuiSystem

#endif // COMMAND_H
