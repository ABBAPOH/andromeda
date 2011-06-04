#include "page.h"

#include <QFileInfo>
#include <guicontroller.h>
#include <perspective.h>

#include "core.h"
#include "perspectivemanager.h"
#include "imainview.h"

using namespace CorePlugin;
using namespace GuiSystem;

Page::Page(QObject *parent) :
    QObject(parent),
    m_history(new History(this))
{
    connect(m_history, SIGNAL(currentItemIndexChanged(int)), SLOT(onIndexChanged(int)));
}

QString Page::currentPath() const
{
    return m_currentPath;
}

void Page::setCurrentPath(QString currentPath)
{
    if (m_currentPath != currentPath) {
        m_currentPath = currentPath;
        openPerspective(m_currentPath);
        emit currentPathChanged(m_currentPath);
    }
}

History *Page::history() const
{
    return m_history;
}

State *Page::state() const
{
    return qobject_cast<State *>(parent());
}

void Page::setState(State *state)
{
    setParent(state);
}

void Page::onIndexChanged(int index)
{
    HistoryItem item = m_history->itemAt(index);
//    if (!item.isValid())
//        return;
    m_currentPath = item.path();
    emit currentPathChanged(m_currentPath);

    openPerspective(item);
}

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return "inode/directory";
    return "";
}

IMainView * Page::getMainView(const QString &perspective)
{
    GuiController *controller = GuiController::instance();
    QString id = controller->perspective(perspective)->property("MainView").toString();

    IView *iview = state()->view(id);
    return qobject_cast<IMainView*>(iview);
}

QString Page::getPerspective(const QString &path)
{
    QString mime = getMimeType(path);
    return Core::instance()->perspectiveManager()->perspective(mime);
}

void Page::openPerspective(const QString &path)
{
    QString perspective = getPerspective(path);
    state()->setCurrentPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(path);
        HistoryItem item = view->currentItem();
        item.setUserData("perspective", perspective);
        if (item.isValid())
            m_history->appendItem(item);
    }
}

void Page::openPerspective(const HistoryItem &item)
{
    QString perspective = item.userData("perspective").toString();
    state()->setCurrentPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(item);
    }
}

