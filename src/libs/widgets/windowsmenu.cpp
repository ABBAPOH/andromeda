#include "windowsmenu.h"

#include <QtGui/QApplication>

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
    QAction *minimizeWindowAction;
    QAction *nextWindowAction;
    QAction *prevWindowAction;
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

    Qt::WindowFlags flags = w->windowFlags();
    if (flags.testFlag(Qt::ToolTip) ||
            flags.testFlag(Qt::Desktop) ||
            flags.testFlag(Qt::Popup) ||
            flags.testFlag(Qt::Drawer))
        return;

    if (widgets.contains(w))
        return;

    QAction *action = new QAction(w->windowTitle(), actionGroup);
    action->setCheckable(true);
    q->insertAction(separatorAction, action);
    QObject::connect(action, SIGNAL(triggered(bool)), q, SLOT(onTriggered(bool)));
    widgets.append(w);
    actions.append(action);

    minimizeWindowAction->setEnabled(true);
    nextWindowAction->setEnabled(widgets.count() > 1);
    prevWindowAction->setEnabled(widgets.count() > 1);
}

void WindowsMenuPrivate::removeWindow(QWidget *w)
{
    if (w->isMinimized())
        return;

    int index = widgets.indexOf(w);
    if (index != -1) {
        widgets.removeAt(index);
        QAction *action = actions.takeAt(index);
        delete action;

        if (actions.isEmpty()) {
            currentIndex = -1;
            minimizeWindowAction->setEnabled(false);
        }
        nextWindowAction->setEnabled(widgets.count() > 1);
        prevWindowAction->setEnabled(widgets.count() > 1);
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

    minimizeWindowAction->setText(WindowsMenu::tr("Minimize"));
    nextWindowAction->setText(WindowsMenu::tr("Next window"));
    prevWindowAction->setText(WindowsMenu::tr("Previous window"));

    q->setTitle(WindowsMenu::tr("Windows"));
}

WindowsMenu::WindowsMenu(QWidget *parent) :
    QMenu(parent),
    d_ptr(new WindowsMenuPrivate(this))
{
    Q_D(WindowsMenu);

    d->minimizeWindowAction = new QAction(this);
    d->minimizeWindowAction->setShortcut(QKeySequence("Ctrl+M"));
    addAction(d->minimizeWindowAction);
    connect(d->minimizeWindowAction, SIGNAL(triggered()), SLOT(minimizeWindow()));

    d->nextWindowAction = new QAction(this);
    d->nextWindowAction->setShortcut(QKeySequence("Ctrl+~"));
    addAction(d->nextWindowAction);
    connect(d->nextWindowAction, SIGNAL(triggered()), SLOT(nextWindow()));

    d->prevWindowAction = new QAction(this);
    d->prevWindowAction->setShortcut(QKeySequence("Ctrl+Shift+~"));
    addAction(d->prevWindowAction);
    connect(d->prevWindowAction, SIGNAL(triggered()), SLOT(prevWindow()));

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

    return d->minimizeWindowAction;
}

QAction *WindowsMenu::nextAction() const
{
    Q_D(const WindowsMenu);

    return d->nextWindowAction;
}

QAction *WindowsMenu::prevAction() const
{
    Q_D(const WindowsMenu);

    return d->prevWindowAction;
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
            } else if (e->type() == QEvent::Hide) {
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
