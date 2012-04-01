#include "windowsmenu.h"

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>

class WindowsMenuPrivate
{
    Q_DECLARE_PUBLIC(WindowsMenu)

public:
    WindowsMenuPrivate(WindowsMenu *qq) : q_ptr(qq) {}

    void addWindow(QWidget *w);
    void removeWindow(QWidget *w);

    void activateWindow(QWidget *w);
    void updateWindow(QWidget *w);
    void activate(int index);

    void retranslateUi();

public:
    QAction *minimizeAction;
    QAction *nextAction;
    QAction *prevAction;
    QWidgetList widgets;
    QList<QAction *> actions;
    QActionGroup *actionGroup;
    int currentIndex;
    QAction *separatorAction;

private:
    WindowsMenu *q_ptr;
};

void WindowsMenuPrivate::addWindow(QWidget *w)
{
    Q_Q(WindowsMenu);

    if (!qobject_cast<QMainWindow*>(w))
        return;

    if (widgets.contains(w))
        return;

    QAction *action = new QAction(w->windowTitle(), actionGroup);
    action->setCheckable(true);
    q->insertAction(separatorAction, action);
    QObject::connect(action, SIGNAL(triggered(bool)), q, SLOT(onTriggered(bool)));
    widgets.append(w);
    actions.append(action);

    minimizeAction->setEnabled(true);
    nextAction->setEnabled(true);
    prevAction->setEnabled(true);
}

void WindowsMenuPrivate::removeWindow(QWidget *w)
{
    int index = widgets.indexOf(w);
    if (index != -1) {
        widgets.removeAt(index);
        QAction *action = actions.takeAt(index);
        delete action;

        if (actions.isEmpty()) {
            currentIndex = -1;
            minimizeAction->setEnabled(false);
            nextAction->setEnabled(false);
            prevAction->setEnabled(false);
        }
    }
}

void WindowsMenuPrivate::activateWindow(QWidget *w)
{
    int index = widgets.indexOf(w);
    if (index != -1) {
        QAction *action = actions[index];
        action->setChecked(true);
        currentIndex = index;
    }
}

void WindowsMenuPrivate::updateWindow(QWidget *w)
{
    int index = widgets.indexOf(w);
    if (index != -1) {
        QAction *action = actions[index];
        action->setText(w->windowTitle());
    }
}

void WindowsMenuPrivate::activate(int index)
{
    if (index != -1) {
        QWidget *w = widgets[index];
        w->raise();
        w->activateWindow();
    }
}

void WindowsMenuPrivate::retranslateUi()
{
    Q_Q(WindowsMenu);

    minimizeAction->setText(WindowsMenu::tr("Minimize"));
    nextAction->setText(WindowsMenu::tr("Next window"));
    prevAction->setText(WindowsMenu::tr("Previous window"));

    q->setTitle(WindowsMenu::tr("Windows"));
}

WindowsMenu::WindowsMenu(QWidget *parent) :
    QMenu(parent),
    d_ptr(new WindowsMenuPrivate(this))
{
    Q_D(WindowsMenu);

    d->minimizeAction = new QAction(this);
    d->minimizeAction->setShortcut(QKeySequence("Ctrl+M"));
    addAction(d->minimizeAction);
    connect(d->minimizeAction, SIGNAL(triggered()), SLOT(minimizeWindow()));

    d->nextAction = new QAction(this);
    d->nextAction->setShortcut(QKeySequence("Ctrl+~"));
    addAction(d->nextAction);
    connect(d->nextAction, SIGNAL(triggered()), SLOT(nextWindow()));

    d->prevAction = new QAction(this);
    d->prevAction->setShortcut(QKeySequence("Ctrl+Shift+~"));
    addAction(d->prevAction);
    connect(d->prevAction, SIGNAL(triggered()), SLOT(prevWindow()));

    addSeparator();

    d->actionGroup = new QActionGroup(this);
    d->currentIndex = -1;
    d->separatorAction = addSeparator();

    foreach (QWidget *w, qApp->topLevelWidgets()) {
        if (w->isVisible() || w->isMinimized())
            d->addWindow(w);
    }

    d->retranslateUi();
    qApp->installEventFilter(this);
}

WindowsMenu::~WindowsMenu()
{
    delete d_ptr;
}

QAction * WindowsMenu::minimizeAction() const
{
    Q_D(const WindowsMenu);

    return d->minimizeAction;
}

QAction *WindowsMenu::nextAction() const
{
    Q_D(const WindowsMenu);

    return d->nextAction;
}

QAction *WindowsMenu::prevAction() const
{
    Q_D(const WindowsMenu);

    return d->prevAction;
}

void WindowsMenu::minimizeWindow()
{
    Q_D(WindowsMenu);

    if (d->currentIndex == -1)
        return;

    QWidget *w = d->widgets[d->currentIndex];
    w->showMinimized();
}

void WindowsMenu::nextWindow()
{
    Q_D(WindowsMenu);

    if (d->currentIndex == -1)
        return;

    int index = (d->currentIndex + 1) % d->widgets.count();
    d->activate(index);
}

void WindowsMenu::prevWindow()
{
    Q_D(WindowsMenu);

    if (d->currentIndex == -1)
        return;

    int index = (d->currentIndex - 1);
    index = index < 0 ? d->widgets.count() - 1: index;
    d->activate(index);
}

void WindowsMenu::onTriggered(bool checked)
{
    Q_D(WindowsMenu);

    if (!checked)
        return; // can't happen

    QAction *action = qobject_cast<QAction*>(sender());

    int index = d->actions.indexOf(action);
    d->activate(index);
}

bool WindowsMenu::eventFilter(QObject *o, QEvent *e)
{
    if (o->isWidgetType()) {
        QWidget *w = static_cast<QWidget *>(o);
        if (w->isWindow()) {
            Q_D(WindowsMenu);

            if (e->type() == QEvent::Show) {
                d->addWindow(w);
            } else if (e->type() == QEvent::Close) {
                d->removeWindow(w);
            } else if (e->type() == QEvent::ActivationChange) {
                d->activateWindow(w);
            } else if (e->type() == QEvent::WindowTitleChange) {
                d->updateWindow(w);
            }
        }
    }

    return QMenu::eventFilter(o, e);
}
