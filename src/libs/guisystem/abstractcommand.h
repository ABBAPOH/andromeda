#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include "guisystem_global.h"

#include <QtCore/QObject>
#include <QtGui/QIcon>

class QAction;

namespace GuiSystem {

class AbstractCommandPrivate;
class GUISYSTEM_EXPORT AbstractCommand : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractCommand)
    Q_DECLARE_PRIVATE(AbstractCommand)

    Q_PROPERTY(QByteArray id READ id)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY changed)
    Q_PROPERTY(QIcon icon READ icon WRITE setIcon NOTIFY changed)
public:
    ~AbstractCommand();

    QByteArray id() const;
    QString text() const;
    QIcon icon() const;

public slots:
    void setText(const QString &text);
    void setIcon(const QIcon &icon);

signals:
    void changed();

protected:
    explicit AbstractCommand(AbstractCommandPrivate &dd, const QByteArray &id, QObject *parent = 0);
    virtual QAction *createAction(QObject *parent = 0) const = 0;

protected:
    AbstractCommandPrivate *d_ptr;

    friend class CommandContainer;
};

} // namespace GuiSystem

#endif // ABSTRACTCOMMAND_H
