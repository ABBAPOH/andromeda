#ifndef PERSPECTIVESWITCHER_H
#define PERSPECTIVESWITCHER_H

#include <QtGui/QToolBar>

namespace GuiSystem {

class State;

class PerspectiveSwitcherPrivate;
class PerspectiveSwitcher : public QToolBar
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(PerspectiveSwitcher)
public:
    explicit PerspectiveSwitcher(QWidget *parent = 0);
    ~PerspectiveSwitcher();

    State *state() const;
    void setState(State *state);

signals:

private slots:
    void onTrigger();

protected:
    PerspectiveSwitcherPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // PERSPECTIVESWITCHER_H
