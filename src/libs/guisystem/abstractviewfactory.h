#ifndef ABSTRACTVIEWFACTORY_H
#define ABSTRACTVIEWFACTORY_H

#include <QtCore/QObject>

namespace GuiSystem {

class AbstractView;
class AbstractViewFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractViewFactory)

public:
    explicit AbstractViewFactory(QObject *parent = 0);
    ~AbstractViewFactory();

    AbstractView *view(QWidget *parent);

    virtual QByteArray id() const = 0;

protected:
    virtual AbstractView *createView(QWidget *parent) = 0;

protected:
    QList<AbstractView *> m_views;

    friend class AbstractView;
};

} // namespace GuiSystem

#endif // ABSTRACTVIEWFACTORY_H
