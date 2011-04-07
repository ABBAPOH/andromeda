#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guisystem_global.h"

#include <QtGui/QMainWindow>

namespace GuiSystem {

class PerspectiveInstance;
class State;

class MainWindowPrivate;
class GUISYSTEM_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    enum ViewAreas { LeftViewArea = 1,
                     TopViewArea = 2,
                     RigthViewArea = 3,
                     BottomViewArea = 4,
                     CentralViewArea = 6,
                   };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    State *currentState() const;

    PerspectiveInstance *perspectiveInstance() const;
    void setPerspectiveInstance(PerspectiveInstance *instance);

signals:

private slots:
    void setPerspective(const QString &id);
    void onDestroy(QObject *object);

protected:
    MainWindowPrivate *d_ptr;

    virtual void displayInstance();

private:
    void createWidgetsForInstance();
};

} // namespace GuiSystem

#endif // MAINWINDOW_H
