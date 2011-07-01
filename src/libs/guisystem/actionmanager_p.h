#ifndef ACTIONMANAGER_P_H
#define ACTIONMANAGER_P_H

#include "actionmanager.h"

#include <QtCore/QMultiMap>
#include <QtCore/QPair>
#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtGui/QMenu>

namespace GuiSystem {

class ActionManagerPrivate : public QObject
{
    Q_OBJECT
public:
    QHash<QString, QObject *> objects;
    QList<Command *> activeCommands;

    QMap<QString, QAction*> mapToAction;
    QMap<QString, QMenu*> mapToMenu;
    QList<QAction*> menuBarActions;

    typedef QPair<QObject*, const char *> ConnectionByObject;
    typedef QPair<QString, const char *> ConnectionById;

    QMultiMap<QString, QPair<QString, const char*> > connectionsViews;
    QMultiMap<QString, QPair<QObject *, const char*> > connectionsObjects;
    QMultiMap<QString, const char*> connectionsToWidgets;

    QMap<QString, QString> mapToView;

public slots:
    void onTrigger(bool);
    void onFocusChange(QWidget * old, QWidget * now);
};

} // namespace GuiSystem

#endif // ACTIONMANAGER_P_H
