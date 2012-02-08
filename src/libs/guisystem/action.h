#ifndef ACTION_H
#define ACTION_H

#include <QtGui/QAction>

namespace GuiSystem {

class Action : public QAction
{
    Q_OBJECT

public:
    Action(const QByteArray &id, QObject *parent);
    Action(const QString &text, const QByteArray &id, QObject *parent);
    Action(const QIcon &icon, const QString &text, const QByteArray &id, QObject* parent);
};

} // namespace GuiSystem

#endif // ACTION_H
