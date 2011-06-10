#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "guisystem_global.h"

#include <QtGui/QMainWindow>

namespace GuiSystem {

class IView;
class PerspectiveInstance;
class State;

class MainWindowPrivate;
class GUISYSTEM_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    enum Area { Left = 0,
                Top = 1,
                Rigth = 2,
                Bottom = 3,
                Central = 4
              };
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    State *currentState() const;
    int currentStateIndex() const;

    QList<State*> states() const;
    int stateCount() const;

signals:

public slots:
    void newTab();
    void setCurrentIndex(int index);
    void closeTab(int index);

private slots:
    void onActionAdd(QAction *action);

protected:
    MainWindowPrivate *d_ptr;

    friend class State;
    friend class StatePrivate;
};

} // namespace GuiSystem

#endif // MAINWINDOW_H
