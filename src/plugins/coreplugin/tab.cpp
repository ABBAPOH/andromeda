#include "tab.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtGui/QStackedLayout>
#include <QtGui/QResizeEvent>

#include "abstracteditor.h"
#include "abstracteditorfactory.h"
#include "core.h"
#include "editormanager.h"

namespace CorePlugin {

class TabPrivate
{
    Q_DECLARE_PUBLIC(Tab)
public:
    TabPrivate(Tab *qq) : q_ptr(qq) {}

    QStackedLayout *layout;
    QString currentPath;
    AbstractEditor *editor;
    QHash<QString, AbstractEditor *> editorHash;
    History *history;
    bool ignoreSignals;

public:
    void setEditor(AbstractEditor *e);
    void addItem(AbstractEditor *e, const QString &path);

protected:
    Tab *q_ptr;
};

} // namespace CorePlugin

using namespace CorePlugin;

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return QLatin1String("inode/directory");
    if (path.startsWith("http://"))
        return QLatin1String("text/html");
    return QString();
}

void TabPrivate::setEditor(AbstractEditor *e)
{
    Q_Q(Tab);

    if (editor) {
        QObject::disconnect(editor, 0, q, 0);
    }

    editor = e;
    QObject::connect(editor, SIGNAL(currentPathChanged(QString)),
                     q, SLOT(onPathChanged(QString)));
    QObject::connect(editor, SIGNAL(iconChanged(QIcon)), q, SIGNAL(changed()));
    QObject::connect(editor, SIGNAL(titleChanged(QString)), q, SIGNAL(changed()));
    QObject::connect(editor, SIGNAL(windowTitleChanged(QString)), q, SIGNAL(changed()));
}

void TabPrivate::addItem(AbstractEditor *e, const QString &path)
{
    if (!e)
        return;

//    QString perspective = perspectiveWidget->instance()->perspective()->id();
    HistoryItem item;
    item.setPath(path);
    item.setIcon(e->icon());
    item.setLastVisited(QDateTime::currentDateTime());
    item.setTitle(e->title());
    item.setUserData(QLatin1String("index"), e->currentIndex());
    item.setUserData(QLatin1String("layoutIndex"), layout->indexOf(e));
//    item.setUserData(QLatin1String("perspective"), perspective);
    history->appendItem(item);
}

Tab::Tab(QWidget *parent) :
    QWidget(parent),
    d_ptr(new TabPrivate(this))
{
    Q_D(Tab);

    d->editor = 0;
    d->layout = new QStackedLayout(this);
    d->history = new History(this);
    d->ignoreSignals = false;

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onIndexChanged(int)));
}

Tab::~Tab()
{
    delete d_ptr;
}

void Tab::open(const QString &path)
{
    Q_D(Tab);

    if (d->currentPath == path)
        return;

    d->currentPath = path;

    QString mimeType = getMimeType(path);
    EditorManager *manager = Core::instance()->editorManager();
    AbstractEditorFactory *factory = manager->factory(mimeType);
    if (factory) {
        QString id = factory->id();
        AbstractEditor *editor = d->editorHash.value(id);
        if (!editor) {
            editor = manager->editor(mimeType, this);
            int index = d->layout->addWidget(editor);
            d->layout->setCurrentIndex(index);
            d->editorHash.insert(id, editor);
        }
        d->setEditor(editor);
        editor->open(path);
    }

}

QString Tab::currentPath() const
{
    return d_func()->currentPath;
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

History * Tab::history() const
{
    return d_func()->history;
}

void Tab::restoreSession(QSettings &s)
{
    Q_D(Tab);

    QString id = s.value(QLatin1String("editor")).toString();
    AbstractEditor *e = Core::instance()->editorManager()->editorById(id, this);
    if (e) {
        d->setEditor(e);
        d->layout->addWidget(e);
        e->restoreSession(s);
    }
}

void Tab::saveSession(QSettings &s)
{
    Q_D(Tab);

    s.setValue(QLatin1String("editor"), d->editor->factory()->id());
    d->editor->saveSession(s);
}

void Tab::onIndexChanged(int index)
{
    Q_D(Tab);

    HistoryItem item = d->history->itemAt(index);

    if (!item.isValid())
        return;

    d->currentPath = item.path();

    d->ignoreSignals = true;

    int layoutIndex = item.userData(QLatin1String("layoutIndex")).toInt();
    d->layout->setCurrentIndex(layoutIndex);
    AbstractEditor *e = qobject_cast<AbstractEditor *>(d->layout->widget(layoutIndex));
    int historyIndex = item.userData(QLatin1String("index")).toInt();
    if (historyIndex != -1) {
        e->setCurrentIndex(historyIndex);
    } else {
        e->open(item.path());
    }

    d->ignoreSignals = false;
    emit currentPathChanged(d->currentPath);
    emit changed();
}

void Tab::onPathChanged(const QString &path)
{
    Q_D(Tab);

    if (d->ignoreSignals)
        return;

    d->addItem(qobject_cast<AbstractEditor*>(sender()), path);
    if (d->currentPath != path) {
        d->currentPath = path;

        emit currentPathChanged(path);
        emit changed();
    }
}
