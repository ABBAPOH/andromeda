#ifndef MAINWINDOWPLUGIN_MAINWINDOW_H
#define MAINWINDOWPLUGIN_MAINWINDOW_H

#include <GuiSystem>

namespace GuiSystem {
class State;
}

namespace CorePlugin {
class GlobalHistory;
}

namespace MainWindowPlugin {

class StateController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentPath READ currentPath WRITE setCurrentPath NOTIFY currentPathChanged)
public:
    explicit StateController(QObject *parent = 0);
    ~StateController();

    QString currentPath() const;
    void setCurrentPath(QString currentPath);

    void back();
    void forward();

    GuiSystem::State *state() const;
    void setState(GuiSystem::State *state);

signals:
    void currentPathChanged(QString currentPath);

private:
    void openPerspective(const QString path, bool addToHistory);
    QString m_currentPath;
    CorePlugin::GlobalHistory *history;
};

class MainWindowPrivate;
class MainWindow : public GuiSystem::MainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MainWindow)
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void back();
    void forward();

protected:
    MainWindowPrivate * d_ptr;
};

} // namespace MainWindowPlugin

#endif // MAINWINDOWPLUGIN_MAINWINDOW_H
