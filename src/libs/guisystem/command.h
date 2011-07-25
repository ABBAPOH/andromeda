#ifndef COMMAND_H
#define COMMAND_H

#include "guisystem_global.h"

#include <QtCore/QObject>
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

    Q_PROPERTY(bool alwaysEnabled READ alwaysEnabled WRITE setAlwaysEnabled)
    Q_PROPERTY(Attributes attributes READ attributes WRITE setAttributes)
    Q_PROPERTY(bool isCheckable READ isCheckable WRITE setCheckable)
    Q_PROPERTY(QKeySequence defaultShortcut READ defaultShortcut WRITE setDefaultShortcut)
    Q_PROPERTY(QIcon defaultIcon READ defaultIcon WRITE setDefaultIcon)
    Q_PROPERTY(QString defaultText READ defaultText WRITE setDefaultText)
    Q_PROPERTY(bool isSeparator READ isSeparator WRITE setSeparator)
    Q_PROPERTY(QString id READ id)

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
    QAction *action(QWidget *w, const char *slot);

    QAction *commandAction() const;

    bool alwaysEnabled() const;
    void setAlwaysEnabled(bool);

    Attributes attributes() const;
    void setAttributes(Attributes attr);

    bool isCheckable() const;
    void setCheckable(bool);

    QKeySequence defaultShortcut() const;
    void setDefaultShortcut(const QKeySequence &key);

    QIcon defaultIcon() const;
    void setDefaultIcon(const QIcon &icon);

    QString defaultText() const;
    void setDefaultText(const QString &text);

    bool isSeparator() const;
    void setSeparator(bool);

    QString id() const;

signals:
    void changed();

private slots:
    void onTrigger(bool);

protected:
    void setRealAction(QAction *commandAction);

protected:
    CommandPrivate *d_ptr;

    friend class ActionManager;
    friend class CommandContainer;
};

} // namespace GuiSystem

#endif // COMMAND_H
