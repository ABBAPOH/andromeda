#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>

namespace GuiSystem {

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit MenuBar(QWidget *parent = 0);

private slots:
    void onActionAdd(QAction *action);
};

} // namespace GuiSystem

#endif // MENUBAR_H
