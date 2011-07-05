#ifndef MAINWINDOW_P_H
#define MAINWINDOW_P_H

#include "mainwindow.h"

#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <enteredlineedit.h>
#include <CorePlugin>

namespace MainWindowPlugin {

class MainWindowPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(MainWindow)
    MainWindow *q_ptr;
public:
    MainWindowPrivate(MainWindow *qq) : q_ptr(qq) {}

    EnteredLineEdit *lineEdit;
    QToolBar *toolBar;
    QTabWidget *tabWidget;

    int currentIndex() const;
    CorePlugin::Tab *currentTab() const;

public slots:
    void onTextEntered(const QString &);
    void onDisplayNameChanged(const QString &);
};

} // namespace MainWindowPlugin

#endif // MAINWINDOW_P_H
