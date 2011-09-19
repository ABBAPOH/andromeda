#include "ieditor.h"

#include "mainwindow.h"

#include <QtCore/QSettings>

using namespace CorePlugin;

/*!
    \class IEditor

    \brief IEditor is a main class for opening urls in Andromeda.
*/

/*!
    \brief Creates a IEditor with the given \a parent.
*/
IEditor::IEditor(QObject *parent) :
    GuiSystem::IView(parent)
{
}

/*!
    \fn bool IEditor::open(const QString &path)

    \brief Reimplement to open content at a \a path.
*/

/*!
    \fn QString IEditor::currentPath() const

    \brief Reimplement to return path currently opened in editor.
*/

/*!
    \fn int IEditor::currentIndex() const

    \brief Reimplement to return current index in local history.

    Default imlementation returns -1, which meanc means that editor doesn't support local history.
*/

/*!
    \fn int IEditor::setCurrentIndex() const

    \brief Reimplement to change current index in local history.

    This funtion is called when user navigates in global history (i.e. with back/forward buttons on toolbar).
*/

/*!
    \fn QIcon IEditor::icon() const

    \brief Reimplement to return current editors icon.
*/

/*!
    \fn QString IEditor::title() const

    \brief Reimplement to return current editors title.

    Value returned by this function normally used as tab title.
*/

/*!
    \fn QString IEditor::windowTitle() const

    \brief Reimplement to return current editors window title.

    Value returned by this function is used as a main window title.
*/

/*!
    \reimp

    Calls open with "currentPath" value in settings.
*/
void IEditor::restoreSession(const QSettings &s)
{
    open(s.value(QLatin1String("currentPath")).toString());
}

/*!
    \reimp

    Stores currentPath with "currentPath" key to settings.
*/
void IEditor::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("currentPath"), currentPath());
}

/*!
    \brief Returns MainWindow that contains this editor.
*/
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
