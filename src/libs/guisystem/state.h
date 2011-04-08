#ifndef STATE_H
#define STATE_H

#include <QtCore/QObject>

namespace GuiSystem {

class MainWindow;
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

    void hideViews();
    void showViews();

    MainWindow *window() const;
    void setWindow(MainWindow *window);

signals:
    void currentPerspectiveChanged(const QString &id);

public slots:

private slots:
    void onFactoryRemoved(const QString &id);

protected:
    StatePrivate *d_ptr;
};

} // namespace GuiSystem

#endif // STATE_H
