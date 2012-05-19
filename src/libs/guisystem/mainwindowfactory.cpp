#include "mainwindowfactory.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QPointer>

#include "mainwindow.h"

using namespace GuiSystem;

QPointer<MainWindowFactory> m_defaultFactory;

MainWindowFactory::MainWindowFactory(QObject *parent) :
    QObject(parent)
{
}

MainWindowFactory *MainWindowFactory::defaultFactory()
{
    return ::m_defaultFactory;
}

void MainWindowFactory::setDefaultfactory(MainWindowFactory *factory)
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

MainWindow * MainWindowFactory::activeWindow() const
{
    return m_activeWindow;
}

MainWindowFactory::Capabilities MainWindowFactory::capabilities() const
{
    return Capabilities(Open | Open);
}

void MainWindowFactory::open(MainWindowFactory::Capability cap, QList<QUrl> urls)
{
    if (urls.isEmpty())
        return;

    switch (cap) {
    case Open: {
        if (urls.count() == 1) {
            if (m_activeWindow) {
                m_activeWindow->open(urls.first());
            } else {
                MainWindow *window = create();
                window->open(urls.first());
                window->show();
            }
        }
        break;
    }
    case OpenNewWindows: {
        foreach (const QUrl &url, urls) {
            MainWindow *window = create();
            window->open(url);
            window->show();
        }
        break;
    }
    default:
        qWarning() << "MainWindowFactory::open :" << "Can't open with" << cap << "capability";
        break;
    }
}

void MainWindowFactory::openEditor(const QString &id)
{
    QUrl url;
    url.setScheme(qApp->applicationName());
    url.setHost(id);
    open(Open, QList<QUrl>() << url);
}

MainWindow * MainWindowFactory::create()
{
    return new MainWindow;
}

bool MainWindowFactory::eventFilter(QObject *object, QEvent *event)
{
    if (object->isWidgetType()) {
        if (event->type() == QEvent::ActivationChange) {
            MainWindow *window = qobject_cast<MainWindow*>(object);
            if (window && window->isActiveWindow()) {
                m_activeWindow = window;
            }
        }
    }
    return QObject::eventFilter(object, event);
}
