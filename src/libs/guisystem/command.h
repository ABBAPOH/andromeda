#ifndef COMMAND_H
#define COMMAND_H

#include <QtCore/QObject>

class QAction;
class QIcon;
class QKeySequence;

namespace GuiSystem {

class CommandPrivate;
class Command : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Command)
    Q_DISABLE_COPY(Command)
public:
    explicit Command(const QString &id, QObject *parent = 0);
    ~Command();

    enum Attribute {
        AttributeHide = 1,
        AttributeUpdateText = 2,
        AttributeUpdateIcon = 4,
        AttributeNonConfigurable = 8
    };
    Q_DECLARE_FLAGS(Attributes, Attribute)

    QAction *action();
    QAction *commandAction() const;

    Attributes attributes() const;
    void setAttributes(Attributes attr);

    QKeySequence defaultShortcut() const;
    void setDefaultShortcut(const QKeySequence &key);

    QIcon defaultIcon() const;
    void setDefaultIcon(const QIcon &icon);

    QString defaultText() const;
    void setDefaultText(const QString &text);

    QString id() const;

signals:

private slots:
    void onTrigger(bool);

protected:
    void setRealAction(QAction *commandAction);

protected:
    CommandPrivate *d_ptr;
    friend class ActionManager;
};

} // namespace GuiSystem

#endif // COMMAND_H
