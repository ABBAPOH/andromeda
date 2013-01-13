#ifndef HISTORYBUTTON_H
#define HISTORYBUTTON_H

#include "../guisystem_global.h"

#include <QtGui/QToolButton>

namespace GuiSystem {

class History;

class HistoryButtonPrivate;
class GUISYSTEM_EXPORT HistoryButton : public QToolButton
{
    Q_OBJECT

public:
    explicit HistoryButton(QWidget *parent = 0);
    ~HistoryButton();

    enum Direction { Back, Forward };

    Direction direction() const;
    void setDirection(Direction direction);

    History *history() const;
    void setHistory(History *history);

signals:

private slots:
    void onAboutToShow();
    void onClicked();
    void onTriggered();

private:
    HistoryButtonPrivate *d;
};

} // namespace GuiSystem

#endif // HISTORYBUTTON_H
