#ifndef WINDOWSMENU_H
#define WINDOWSMENU_H

#include <QtGui/QMenu>

class WindowsMenuPrivate;
class WindowsMenu : public QMenu
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowsMenu)

public:
    explicit WindowsMenu(QWidget *parent = 0);
    ~WindowsMenu();

    QAction *minimizeAction() const;
    QAction *nextAction() const;
    QAction *prevAction() const;

public slots:
    void minimizeWindow();
    void nextWindow();
    void prevWindow();

private slots:
    void onTriggered(bool checked);

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    WindowsMenuPrivate *d_ptr;
};

#endif // WINDOWSMENU_H
