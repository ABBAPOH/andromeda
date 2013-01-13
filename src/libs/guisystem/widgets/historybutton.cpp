#include "historybutton.h"

#include <QtGui/QMenu>

#include "history.h"
#include "historyitem.h"

namespace GuiSystem {

class HistoryButtonPrivate
{
public:
    HistoryButton::Direction direction;
    History *history;
};

} // namespace GuiSystem

using namespace GuiSystem;

static inline QString urlToUserString(const QUrl &url)
{
    if (url.scheme() == QLatin1String("file"))
        return url.toLocalFile();
    else
        return url.toString();
}


HistoryButton::HistoryButton(QWidget *parent) :
    QToolButton(parent),
    d(new HistoryButtonPrivate)
{
    d->direction = Back;
    d->history = 0;

    QMenu *menu = new QMenu(this);
    menu->addAction("lol");
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(onAboutToShow()));
    setMenu(menu);
    setPopupMode(QToolButton::DelayedPopup);

    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

HistoryButton::~HistoryButton()
{
    delete d;
}

HistoryButton::Direction HistoryButton::direction() const
{
    return d->direction;
}

void HistoryButton::setDirection(HistoryButton::Direction direction)
{
    d->direction = direction;
}

History *HistoryButton::history() const
{
    return d->history;
}

void HistoryButton::setHistory(History *history)
{
    d->history = history;
}

void HistoryButton::onAboutToShow()
{
    QMenu *menu = qobject_cast<QMenu*>(sender());
    menu->clear();

    QString previousEditor;
    if (d->direction == Back) {
        for (int i = d->history->currentItemIndex() - 1; i >= 0 ; i--) {
            HistoryItem item = d->history->itemAt(i);

            QString editor = item.userData(QLatin1String("editor")).toString();
            if (previousEditor != editor)
                menu->addSeparator();
            previousEditor = editor;

            QAction *action = new QAction(menu);
            action->setText(urlToUserString(item.url()));
            action->setIcon(item.icon());
            action->setData(i);
            menu->addAction(action);
            connect(action, SIGNAL(triggered()), SLOT(onTriggered()));
        }
    } else {
        for (int i = d->history->currentItemIndex() + 1; i < d->history->count() ; i++) {
            HistoryItem item = d->history->itemAt(i);

            QString editor = item.userData(QLatin1String("editor")).toString();
            if (previousEditor != editor)
                menu->addSeparator();
            previousEditor = editor;

            QAction *action = new QAction(menu);
            action->setText(urlToUserString(item.url()));
            action->setIcon(item.icon());
            action->setData(i);
            menu->addAction(action);
            connect(action, SIGNAL(triggered()), SLOT(onTriggered()));
        }
    }
}

void HistoryButton::onClicked()
{
    if (!d->history)
        return;

    if (d->direction == Back)
        d->history->back();
    else
        d->history->forward();
}

void HistoryButton::onTriggered()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (!action)
        return;

    if (!d->history)
        return;

    int index = action->data().toInt();
    d->history->setCurrentItemIndex(index);
}
