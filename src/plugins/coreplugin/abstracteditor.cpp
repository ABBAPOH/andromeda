#include "abstracteditor.h"
#include "abstracteditorfactory.h"
#include "mainwindow.h"

#include <QtCore/QSettings>

using namespace CorePlugin;

/*!
    \class AbstractEditor

    \brief AbstractEditor is a main class for opening urls in Andromeda.
*/

/*!
    \brief Creates a AbstractEditor with the given \a parent.
*/
AbstractEditor::AbstractEditor(QWidget *parent) :
    QWidget(parent),
    m_factory(0)
{
}

/*!
    \brief Destroys AbstractEditor and removes it from factory.
*/
AbstractEditor::~AbstractEditor()
{
    m_factory->m_editors.removeAll(this);
}

/*!
    \brief Returns factory that created this editor.
*/
AbstractEditorFactory * AbstractEditor::factory() const
{
    return m_factory;
}

/*!
    \internal
*/
void AbstractEditor::setFactory(AbstractEditorFactory *factory)
{
    m_factory = factory;
    factory->m_editors.append(this);
}

/*!
    \fn bool AbstractEditor::open(const QString &path)

    \brief Reimplement to open content at a \a path.
*/

/*!
    \fn QString AbstractEditor::currentPath() const

    \brief Reimplement to return path currently opened in editor.
*/

/*!
    \fn int AbstractEditor::currentIndex() const

    \brief Reimplement to return current index in local history.

    Default imlementation returns -1, which meanc means that editor doesn't support local history.
*/

/*!
    \fn int AbstractEditor::setCurrentIndex() const

    \brief Reimplement to change current index in local history.

    This funtion is called when user navigates in global history (i.e. with back/forward buttons on toolbar).
*/

/*!
    \fn QIcon AbstractEditor::icon() const

    \brief Reimplement to return current editors icon.
*/

/*!
    \fn QString AbstractEditor::title() const

    \brief Reimplement to return current editors title.

    Value returned by this function normally used as tab title.
*/

/*!
    \fn QString AbstractEditor::windowTitle() const

    \brief Reimplement to return current editors window title.

    Value returned by this function is used as a main window title.
*/

/*!
    \reimp

    Calls open with "currentPath" value in settings.
*/
void AbstractEditor::restoreSession(QSettings &s)
{
    open(s.value(QLatin1String("currentPath")).toString());
}

/*!
    \reimp

    Stores currentPath with "currentPath" key to settings.
*/
void AbstractEditor::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("currentPath"), currentPath());
}

/*!
    \brief Returns MainWindow that contains this editor.
*/
MainWindow * AbstractEditor::mainWindow() const
{
    QWidget *w = (QWidget *)this;
    while (w->parentWidget()) {
        w = w->parentWidget();
    }

    MainWindow *window = qobject_cast<MainWindow*>(w);
    Q_ASSERT_X(window, "IEditor::mainWindow", "Attempt to use IEditor not within MainWindow");
    return window;
}

AbstractEditor * AbstractEditor::currentEditor()
{
    return MainWindow::currentWindow()->currentEditor();
}
