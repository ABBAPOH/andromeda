#ifndef ABSTRACTCOMMAND_P_H
#define ABSTRACTCOMMAND_P_H

#include "abstractcommand.h"

namespace GuiSystem {

class AbstractCommandPrivate
{
    Q_DECLARE_PUBLIC(AbstractCommand)
public:
    AbstractCommandPrivate(AbstractCommand *qq) : q_ptr(qq) {}
    virtual ~AbstractCommandPrivate() {}

    virtual void setText(const QString &text);
    virtual void setIcon(const QIcon &icon);

    QByteArray id;
    QString text;
    QIcon icon;

protected:
    AbstractCommand *q_ptr;
};

} // namespace GuiSystem

#endif // ABSTRACTCOMMAND_P_H
