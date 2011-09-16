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

inline uint qHash(const CorePlugin::HistoryItem &item) { return qHash(item.path()); }

class TabPrivate
{
    Q_DECLARE_PUBLIC(Tab)
public:
    TabPrivate(Tab *qq) : q_ptr(qq) {}

    IEditor *getEditor(const QString &perspective);
    QString getPerspective(const QString &path);
    bool openPerspective(const QString &path);
    void openPerspective(const HistoryItem &item);
    void setEditor(IEditor *e);
    void addItem(IEditor *e, const QString &path);

    GuiSystem::PerspectiveWidget *perspectiveWidget;
    QString currentPath;
    History *history;
    bool ignoreSignals;
    IEditor *editor;
    QHash<HistoryItem, QString> mapToPerspective;

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
    emit changed();
}

void Tab::onPathChanged(const QString &path)
{
    Q_D(Tab);

    if (d->ignoreSignals)
        return;

    d->addItem(qobject_cast<IEditor*>(sender()), path);
    d->currentPath = path;

    emit currentPathChanged(path);
}

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return QLatin1String("inode/directory");
    if (path.startsWith("http://"))
        return QLatin1String("text/html");
    return QString();
}

IEditor * TabPrivate::getEditor(const QString &perspective)
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

    IEditor *e = getEditor(perspective);
    if (e) {
        setEditor(e);
        e->open(path);
        addItem(e, path);
    }
    return true;
}

void TabPrivate::openPerspective(const HistoryItem &item)
{
    QString perspective = item.userData(QLatin1String("perspective")).toString();
    perspectiveWidget->openPerspective(perspective);

    IEditor *e = getEditor(perspective);
    if (e) {
        editor = e;
        int index = item.userData(QLatin1String("index")).toInt();
        if (index != -1) {
            e->setCurrentIndex(index);
        } else {
            e->open(item.path());
        }
    }
}

void TabPrivate::setEditor(CorePlugin::IEditor *e)
{
    Q_Q(Tab);

    editor = e;
    QObject::connect(editor, SIGNAL(pathChanged(QString)), q,
                     SLOT(onPathChanged(QString)), Qt::UniqueConnection);
    QObject::connect(editor, SIGNAL(changed()), q, SIGNAL(changed()), Qt::UniqueConnection);
}

void TabPrivate::addItem(IEditor *e, const QString &path)
{
    if (!e)
        return;

    QString perspective = perspectiveWidget->instance()->perspective()->id();
    HistoryItem item;
    item.setPath(path);
    item.setIcon(e->icon());
    item.setLastVisited(QDateTime::currentDateTime());
    item.setTitle(e->title());
    item.setUserData(QLatin1String("index"), e->currentIndex());
    item.setUserData(QLatin1String("perspective"), perspective);
    history->appendItem(item);
}

Tab::Tab(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TabPrivate(this))
{
    Q_D(Tab);

    d->history = new History(this);
    d->perspectiveWidget = new PerspectiveWidget(this);
    d->ignoreSignals = false;
    d->editor = 0;

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

void Tab::open(const QString & currentPath)
{
    Q_D(Tab);

    if (d->ignoreSignals)
        return;

    d->ignoreSignals = true;

    if (d->currentPath != currentPath) {
        d->currentPath = currentPath;
        if (d->openPerspective(d->currentPath)) {
            d->ignoreSignals = false;
            emit currentPathChanged(d->currentPath);
            emit changed();
        }
    }
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
    IEditor *editor = d->getEditor(perspective);
    if (editor) {
        d->currentPath = editor->currentPath();
        d->setEditor(editor);
        d->addItem(editor, d->currentPath);

        emit currentPathChanged(d->currentPath);
        emit changed();
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

QIcon Tab::icon() const
{
    Q_D(const Tab);

    if (d->editor)
        return d->editor->icon();

    return QIcon();
}

QString Tab::title() const
{
    Q_D(const Tab);

    if (d->editor)
        return d->editor->title();

    return QString();
}

QString Tab::windowTitle() const
{
    Q_D(const Tab);

    if (d->editor)
        return d->editor->windowTitle();

    return QString();
}
