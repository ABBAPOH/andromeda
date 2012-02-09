#ifndef ACTION_H
#define ACTION_H

#include "guisystem_global.h"

#include <QtGui/QAction>

namespace GuiSystem {

class GUISYSTEM_EXPORT Action : public QAction
{
    Q_OBJECT

public:
    Action(const QByteArray &id, QObject *parent);
    Action(const QString &text, const QByteArray &id, QObject *parent);
    Action(const QIcon &icon, const QString &text, const QByteArray &id, QObject* parent);
};

} // namespace GuiSystem

#endif // ACTION_H
