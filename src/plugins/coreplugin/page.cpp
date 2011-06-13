#include "page.h"

#include <QFileInfo>
#include <guicontroller.h>
#include <perspective.h>
#include <perspectiveinstance.h>

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

PerspectiveWidget * Page::perspectiveWidget() const
{
    return qobject_cast<PerspectiveWidget *>(parent());
}

void Page::setPerspectiveWidget(PerspectiveWidget *w)
{
    setParent(w);
}

void Page::onIndexChanged(int index)
{
    HistoryItem item = m_history->itemAt(index);

    if (!item.isValid())
        return;

    m_currentPath = item.path();

    openPerspective(item);
    emit currentPathChanged(m_currentPath);
}

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return QLatin1String("inode/directory");
    return QString();
}

IMainView * Page::getMainView(const QString &perspective)
{
    GuiController *controller = GuiController::instance();
    QString id = controller->perspective(perspective)->property("MainView").toString();

    return qobject_cast<IMainView*>(perspectiveWidget()->currentInstance()->view(id));
}

QString Page::getPerspective(const QString &path)
{
    QString mime = getMimeType(path);
    return Core::instance()->perspectiveManager()->perspective(mime);
}

void Page::openPerspective(const QString &path)
{
    QString perspective = getPerspective(path);
    perspectiveWidget()->openPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(path);
        connect(view, SIGNAL(pathChanged(QString)), SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        HistoryItem item = view->currentItem();
        item.setUserData("perspective", perspective);
        m_history->appendItem(item);

    }
}

void Page::openPerspective(const HistoryItem &item)
{
    QString perspective = item.userData("perspective").toString();
    perspectiveWidget()->openPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(item);
    }
}


