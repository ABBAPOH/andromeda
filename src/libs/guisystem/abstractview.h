#ifndef ABSTRACTVIEW_H
#define ABSTRACTVIEW_H

#include "guisystem_global.h"

#include <QtGui/QIcon>
#include <QtGui/QWidget>

namespace GuiSystem {

class ActionManager;
class AbstractViewFactory;

class AbstractViewPrivate;
class GUISYSTEM_EXPORT AbstractView : public QWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractView)
    Q_DISABLE_COPY(AbstractView)

    Q_PROPERTY(QByteArray id READ id)
    Q_PROPERTY(QIcon icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)

public:
    explicit AbstractView(QWidget *parent = 0);
    ~AbstractView();

    QByteArray id() const;

    void addAction(QAction *action, const QByteArray &id);

    virtual QIcon icon() const;
    virtual QString title() const;

    virtual void restoreState(QByteArray &state);
    virtual QByteArray saveState() const;

signals:
    void iconChanged(const QIcon &icon);
    void titleChanged(const QString &title);

protected:
    AbstractViewFactory *factory() const;
    void setFactory(AbstractViewFactory *factory);

    ActionManager *actionManager() const;
    void registerAction(QAction *action, const QByteArray &id);

private:
    AbstractViewFactory *m_factory;

    friend class AbstractViewFactory;
};

} // namespace GuiSystem

#endif // ABSTRACTVIEW_H
