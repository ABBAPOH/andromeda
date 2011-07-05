#include "tab.h"

#include <QtCore/QFileInfo>
#include <QtGui/QResizeEvent>
#include <guicontroller.h>
#include <perspective.h>
#include <perspectiveinstance.h>

#include "core.h"
#include "perspectivemanager.h"
#include "imainview.h"

namespace CorePlugin {

class TabPrivate
{
    Q_DECLARE_PUBLIC(Tab)
public:
    TabPrivate(Tab *qq) : q_ptr(qq) {}

    IMainView *getMainView(const QString &perspective);
    QString getPerspective(const QString &path);
    void openPerspective(const QString &path);
    void openPerspective(const HistoryItem &item);

    GuiSystem::PerspectiveWidget *perspectiveWidget;
    QString currentPath;
    History *history;

protected:
    Tab *q_ptr;
};

} // namespace CorePlugin

using namespace CorePlugin;
using namespace GuiSystem;

void Tab::onIndexChanged(int index)
{
    Q_D(Tab);

    HistoryItem item = d->history->itemAt(index);

    if (!item.isValid())
        return;

    d->currentPath = item.path();

    d->openPerspective(item);
    emit currentPathChanged(d->currentPath);
}

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return QLatin1String("inode/directory");
    return QString();
}

IMainView * TabPrivate::getMainView(const QString &perspective)
{
    GuiController *controller = GuiController::instance();
    QString id = controller->perspective(perspective)->property("MainView").toString();

    return qobject_cast<IMainView*>(perspectiveWidget->currentInstance()->view(id));
}

QString TabPrivate::getPerspective(const QString &path)
{
    QString mime = getMimeType(path);
    return Core::instance()->perspectiveManager()->perspective(mime);
}

void TabPrivate::openPerspective(const QString &path)
{
    QString perspective = getPerspective(path);
    perspectiveWidget->openPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(path);
        QObject::connect(view, SIGNAL(pathChanged(QString)), q_func(),
                         SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        HistoryItem item = view->currentItem();
        item.setUserData("perspective", perspective);
        history->appendItem(item);
    }
}

void TabPrivate::openPerspective(const HistoryItem &item)
{
    QString perspective = item.userData("perspective").toString();
    perspectiveWidget->openPerspective(perspective);

    IMainView *view = getMainView(perspective);
    if (view) {
        view->open(item);
    }
}

Tab::Tab(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TabPrivate(this))
{
    Q_D(Tab);

    d->history = new History(this);
    d->perspectiveWidget = new PerspectiveWidget(this);

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onIndexChanged(int)));
}

Tab::~Tab()
{
    delete d_ptr;
}

QString Tab::currentPath() const
{
    return d_func()->currentPath;
}

void Tab::setCurrentPath(const QString & currentPath)
{
    Q_D(Tab);

    if (d->currentPath != currentPath) {
        d->currentPath = currentPath;
        d->openPerspective(d->currentPath);
        emit currentPathChanged(d->currentPath);
    }
}

History *Tab::history() const
{
    return d_func()->history;
}

PerspectiveWidget * Tab::perspectiveWidget() const
{
    return d_func()->perspectiveWidget;
}

void Tab::resizeEvent(QResizeEvent *e)
{
    d_func()->perspectiveWidget->resize(e->size());
}
