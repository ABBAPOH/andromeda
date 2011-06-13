#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QtCore/QObject>

class QAction;
class QMenu;

typedef QList<QAction*> QActionList;

namespace GuiSystem {

class ActionManagerPrivate;
class ActionManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ActionManager)
public:
    explicit ActionManager(QObject *parent = 0);
    ~ActionManager();

    static ActionManager *instance();

    void addAction(QAction *action);
    QAction *addMenu(QMenu *menu);
    void addSeparator();

    QActionList actions() const;
    QActionList actions(const QString &viewId) const;

    QAction *action(const QString &id) const;
    QMenu *menu(const QString &id) const;

    void registerAction(QAction *action, const QString &id);
    void registerMenu(QMenu *menu, const QString &id);

    void unregister(const QString &id);

    bool connect(const QString &actionId, const QString &viewId, const char *slot);
    bool connect(const QString &actionId, QObject *receiver, const char *slot);
    bool connect(const QString &actionId, const char *slot);

    void disconnect(const QString &actionId, const QString &viewId, const char *slot);
    void disconnect(const QString &actionId, QObject *receiver, const char *slot);
    void disconnect(const QString &actionId, const char *slot);

signals:
    void actionAdded(QAction *action);

public slots:

protected:
    ActionManagerPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // ACTIONMANAGER_H
