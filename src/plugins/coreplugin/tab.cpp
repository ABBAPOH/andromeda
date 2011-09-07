#include "tab.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtGui/QResizeEvent>
#include <QtGui/QDesktopServices>
#include <guicontroller.h>
#include <perspective.h>
#include <perspectiveinstance.h>

#include "core.h"
#include "ieditor.h"
#include "perspectivemanager.h"

namespace CorePlugin {

class TabPrivate
{
    Q_DECLARE_PUBLIC(Tab)
public:
    TabPrivate(Tab *qq) : q_ptr(qq) {}

    IEditor *getMainView(const QString &perspective);
    QString getPerspective(const QString &path);
    bool openPerspective(const QString &path);
    void openPerspective(const HistoryItem &item);

    GuiSystem::PerspectiveWidget *perspectiveWidget;
    QString currentPath;
    History *history;
    bool ignoreSignals;

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

    d->ignoreSignals = true;
    d->openPerspective(item);
    d->ignoreSignals = false;
    emit currentPathChanged(d->currentPath);
    emit displayNameChanged(displayName());
}

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return QLatin1String("inode/directory");
    return QString();
}

IEditor * TabPrivate::getMainView(const QString &perspective)
{
    GuiController *controller = GuiController::instance();
    QString id = controller->perspective(perspective)->property("MainView").toString();

    return qobject_cast<IEditor*>(perspectiveWidget->instance()->view(id));
}

QString TabPrivate::getPerspective(const QString &path)
{
    QString mime = getMimeType(path);
    return Core::instance()->perspectiveManager()->perspective(mime);
}

bool TabPrivate::openPerspective(const QString &path)
{
    QString perspective = getPerspective(path);
    if (perspective.isEmpty()) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        return false;
    }

    perspectiveWidget->openPerspective(perspective);

    IEditor *view = getMainView(perspective);
    if (view) {
        view->open(path);
        QObject::connect(view, SIGNAL(pathChanged(QString)), q_func(),
                         SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        QObject::connect(view, SIGNAL(openRequested(QString)), q_func(),
                         SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        HistoryItem item = view->currentItem();
        item.setUserData("perspective", perspective);
        history->appendItem(item);
    }
    return true;
}

void TabPrivate::openPerspective(const HistoryItem &item)
{
    QString perspective = item.userData("perspective").toString();
    perspectiveWidget->openPerspective(perspective);

    IEditor *view = getMainView(perspective);
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
    d->ignoreSignals = false;

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

    if (d->ignoreSignals)
        return;

    d->ignoreSignals = true;

//    if (d->currentPath != currentPath) {
        d->currentPath = currentPath;
        if (d->openPerspective(d->currentPath)) {
            d->ignoreSignals = false;
            emit currentPathChanged(d->currentPath);
            emit displayNameChanged(displayName());
        }
//    }
    d->ignoreSignals = false;
}

History *Tab::history() const
{
    return d_func()->history;
}

PerspectiveWidget * Tab::perspectiveWidget() const
{
    return d_func()->perspectiveWidget;
}

void Tab::restoreSession(QSettings &s)
{
    Q_D(Tab);

    d->perspectiveWidget->restoreSession(s);

    QString perspective = d->perspectiveWidget->perspective()->id();
    IEditor *view = d->getMainView(perspective);
    if (view) {
        d->currentPath = view->currentPath();
        emit currentPathChanged(d->currentPath);
        emit displayNameChanged(displayName());

        QObject::connect(view, SIGNAL(pathChanged(QString)), this,
                         SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        QObject::connect(view, SIGNAL(openRequested(QString)), this,
                         SLOT(setCurrentPath(QString)), Qt::UniqueConnection);
        HistoryItem item = view->currentItem();
        item.setUserData("perspective", perspective);
        d->history->appendItem(item);
    }
}

void Tab::saveSession(QSettings &s)
{
    Q_D(Tab);

    d->perspectiveWidget->saveSession(s);
}

void Tab::resizeEvent(QResizeEvent *e)
{
    d_func()->perspectiveWidget->resize(e->size());
}

QString Tab::displayName() const
{
    QFileInfo fi(currentPath());
    if (fi.exists())
        return fi.fileName();
    return QString();
}
