#ifndef STATE_H
#define STATE_H

#include <QtCore/QObject>

namespace GuiSystem {

class PerspectiveInstance;

class StatePrivate;
class State : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(State)
public:
    explicit State(QObject *parent = 0);
    ~State();

    PerspectiveInstance *currentInstance() const;

    void setCurrentPerspective(const QString &id);
//    void closeInstance(const QString &id);

    QStringList perspectiveIds() const;
    QStringList availablePerspectives() const;

signals:
    void currentPerspectiveChanged(const QString &id);

public slots:

private slots:
    void onDestroy(QObject *object); // TODO : move to private class OR manually remove from View's destructor

protected:
    StatePrivate *d_ptr;
};

} // namespace GuiSystem

#endif // STATE_H
