#include "openstrategies.h"

#include <QApplication>

#include <Parts/AbstractDocumentFactory>
#include <Parts/constants.h>
#include <Parts/DocumentManager>

#include "browserwindow.h"

using namespace Parts;
using namespace Andromeda;

/*!
    \class OpenCurrentWindowStrategy
    Opens urls in current window.
*/
OpenCurrentWindowStrategy::OpenCurrentWindowStrategy(QObject *parent) :
    OpenStrategy(Constants::Actions::Open, parent)
{
}

bool OpenCurrentWindowStrategy::canOpen(const QList<QUrl> &urls)
{
    return !urls.isEmpty();
}

#include <QtCore/QFileInfo>
#include <QtGui/QDesktopServices>
void OpenCurrentWindowStrategy::open(const QList<QUrl> &urls)
{
    QList<QUrl> folders;
    foreach (const QUrl &url, urls) {
        if (!url.isLocalFile()) {
            folders.append(url);
            continue;
        }
        Q_ASSERT(url.isLocalFile());
        QString path = url.toLocalFile();
        QFileInfo info(path);
        if (info.isDir() && !info.isBundle()) {
            folders.append(url);
        } else {
#ifdef Q_OS_LINUX
            if (info.isExecutable()) {
                QProcess::startDetached(path);
                return;
            }
#endif
            // TODO: allow to open default editor instead
            QDesktopServices::openUrl(url);
        }
    }

    if (folders.isEmpty())
        return;

    BrowserWindow *m_activeWindow = qobject_cast<BrowserWindow *>(QApplication::activeWindow());
    if (!m_activeWindow)
        m_activeWindow = new BrowserWindow;

    // TODO: open files in external program
    if (folders.count() == 1)
        m_activeWindow->open(folders.first());
    else // TODO: switch between new tabs/windows
        m_activeWindow->openNewTabs(folders);

    m_activeWindow->show();
}

QString OpenCurrentWindowStrategy::text() const
{
    return tr("Open");
}

/*!
    \class OpenNewTabStrategy
    Opens urls in new tabs in current window.
*/
OpenNewTabStrategy::OpenNewTabStrategy(QObject *parent) :
    OpenStrategy(Constants::Actions::OpenInTab, parent)
{
}

bool OpenNewTabStrategy::canOpen(const QList<QUrl> &urls)
{
    return !urls.isEmpty();
}

void OpenNewTabStrategy::open(const QList<QUrl> &urls)
{
    BrowserWindow *m_activeWindow = qobject_cast<BrowserWindow *>(QApplication::activeWindow());
    if (!m_activeWindow)
        m_activeWindow = new BrowserWindow;

    if (urls.count() == 1)
        m_activeWindow->openNewTab(urls.first());
    else
        m_activeWindow->openNewTabs(urls);

    m_activeWindow->show();
}

QString OpenNewTabStrategy::text() const
{
    return tr("Open in tab");
}

Qt::KeyboardModifiers OpenNewTabStrategy::modifiers() const
{
    return Qt::AltModifier;
}

/*!
    \class OpenNewWindowStrategy
    Opens urls in new windows.
*/
OpenNewWindowStrategy::OpenNewWindowStrategy(QObject *parent) :
    OpenStrategy(Constants::Actions::OpenInWindow, parent)
{
}

bool OpenNewWindowStrategy::canOpen(const QList<QUrl> &urls)
{
    return !urls.isEmpty();
}

void OpenNewWindowStrategy::open(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        BrowserWindow *window = new BrowserWindow;
        window->open(url);
        window->show();
    }
}

QString OpenNewWindowStrategy::text() const
{
    return tr("Open in window");
}

Qt::KeyboardModifiers OpenNewWindowStrategy::modifiers() const
{
    return Qt::ControlModifier | Qt::AltModifier;
}

/*!
    \class OpenEditorStrategy
    Opens urls in an editor.
*/
OpenEditorStrategy::OpenEditorStrategy(QObject *parent):
    OpenStrategy("OpenEditor", parent)
{
}

bool OpenEditorStrategy::canOpen(const QList<QUrl> &urls)
{
    // TODO: return false for directories
    return !urls.isEmpty();
}

void OpenEditorStrategy::open(const QList<QUrl> &urls)
{
    DocumentManager *manager = DocumentManager::instance();
    QList<AbstractDocumentFactory *> factories = manager->factoriesForUrls(urls);
    if (factories.isEmpty())
        return;

    BrowserWindow *m_activeWindow = qobject_cast<BrowserWindow *>(QApplication::activeWindow());
    if (!m_activeWindow)
        m_activeWindow = new BrowserWindow;

    // TODO: handle multiple editors via dialog
    m_activeWindow->openEditor(urls, factories.first()->id());

    m_activeWindow->show();
}

QString OpenEditorStrategy::text() const
{
    return tr("Open in editor");
}

QKeySequence OpenEditorStrategy::keySequence() const
{
    return QKeySequence("Ctrl+E");
}
