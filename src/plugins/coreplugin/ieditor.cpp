#include "ieditor.h"

#include "mainwindow.h"

#include <QtCore/QSettings>

using namespace CorePlugin;

IEditor::IEditor(QObject *parent) :
    GuiSystem::IView(parent)
{
}

QString IEditor::currentPath() const
{
    return currentItem().path();
}

void IEditor::restoreSession(const QSettings &s)
{
    open(s.value(QLatin1String("currentPath")).toString());
}

void IEditor::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("currentPath"), currentPath());
}

MainWindow * IEditor::mainWindow() const
{
    QWidget *w = widget();
    while (w->parentWidget()) {
        w = w->parentWidget();
    }

    MainWindow *window = qobject_cast<MainWindow*>(w);
    Q_ASSERT_X(window, "IEditor::mainWindow", "Attempt to use IEditor not within MainWindow");
    return window;
}
