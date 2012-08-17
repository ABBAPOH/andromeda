#include "editorwindowfactory.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>

#include "editorwindow.h"

using namespace GuiSystem;

QPointer<EditorWindowFactory> m_defaultFactory;

/*!
    \class GuiSystem::AbstractEditor
*/

EditorWindowFactory::EditorWindowFactory(QObject *parent) :
    QObject(parent)
{
}

EditorWindowFactory *EditorWindowFactory::defaultFactory()
{
    return ::m_defaultFactory;
}

void EditorWindowFactory::setDefaultfactory(EditorWindowFactory *factory)
{
    Q_ASSERT_X(qApp, "MainWindowFactory::setDefaultfactory", "Must construct a QApplication before calling this method");

    if (::m_defaultFactory == factory)
        return;

    if (::m_defaultFactory) {
        qApp->removeEventFilter(::m_defaultFactory);
    }

    ::m_defaultFactory = factory;
    qApp->installEventFilter(::m_defaultFactory);
}

EditorWindow * EditorWindowFactory::activeWindow() const
{
    return m_activeWindow;
}

EditorWindowFactory::OpenFlags EditorWindowFactory::openFlags() const
{
    return OpenFlags(Open | OpenNewWindows);
}

void EditorWindowFactory::open(const QUrl &url)
{
    open(QList<QUrl>() << url);
}

void EditorWindowFactory::open(const QList<QUrl> &urls)
{
//    openFlag(Open, urls);
    if (urls.count() == 1) {
        if (m_activeWindow) {
            m_activeWindow->open(urls.first());
        } else {
            EditorWindow *window = create();
            window->open(urls.first());
            window->show();
        }
    } else {
        openNewWindows(urls);
    }
}

void EditorWindowFactory::openNewEditor(const QUrl &url)
{
    openNewEditor(QList<QUrl>() << url);
}

void EditorWindowFactory::openNewEditor(const QList<QUrl> &urls)
{
    qWarning() << "EditorWindowFactory::openNewEditor" << "is not implemented";
}

void EditorWindowFactory::openNewWindow(const QUrl &url)
{
    openNewWindows(QList<QUrl>() << url);
}

void EditorWindowFactory::openNewWindow(const QList<QUrl> &urls)
{
    openNewWindows(urls);
}

void EditorWindowFactory::openNewWindows(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        EditorWindow *window = create();
        window->open(url);
        window->show();
    }
}

void EditorWindowFactory::openEditor(const QString &id)
{
    QUrl url;
    url.setScheme(qApp->applicationName());
    url.setHost(id);
    open(QList<QUrl>() << url);
}

EditorWindow * EditorWindowFactory::create()
{
    return new EditorWindow;
}

bool EditorWindowFactory::eventFilter(QObject *object, QEvent *event)
{
    if (object->isWidgetType()) {
        if (event->type() == QEvent::ActivationChange) {
            EditorWindow *window = qobject_cast<EditorWindow*>(object);
            if (window && window->isActiveWindow()) {
                m_activeWindow = window;
            }
        }
    }
    return QObject::eventFilter(object, event);
}
