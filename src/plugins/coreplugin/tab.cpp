#include "tab.h"

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QStackedLayout>
#include <QtGui/QResizeEvent>
#include <QtGui/QDesktopServices>

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
    QUrl currentUrl;
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

QString getMimeType(const QUrl &url)
{
    if (url.scheme() == QLatin1String("file")) {
        QFileInfo info(url.toLocalFile());
        if (info.isDir() && !info.isBundle())
            return QLatin1String("inode/directory");
    } else if(url.scheme() == QLatin1String("http")) {
        return QLatin1String("text/html");
    } else if(url.scheme() == QLatin1String("about") && url.path() == QLatin1String("helloworld")) {
        return QLatin1String("application/helloworld");
    }
    return QString();
}

void TabPrivate::setEditor(AbstractEditor *e)
{
    Q_Q(Tab);

    if (editor) {
        QObject::disconnect(editor, 0, q, 0);
    }

    editor = e;
    QObject::connect(editor, SIGNAL(currentUrlChanged(QUrl)),
                     q, SLOT(onUrlChanged(QUrl)));
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

QUrl Tab::currentUrl() const
{
    return d_func()->currentUrl;
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

AbstractEditor * Tab::currentEditor() const
{
    return d_func()->editor;
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

void Tab::open(const QUrl &url)
{
    Q_D(Tab);

    if (url.isEmpty())
        return;

    if (d->currentUrl == url)
        return;

    QString mimeType = getMimeType(url);
    EditorManager *manager = Core::instance()->editorManager();
    AbstractEditorFactory *factory = manager->factory(mimeType);
    if (factory) {
        QString id = factory->id();
        AbstractEditor *editor = d->editorHash.value(id);
        if (!editor) {
            editor = manager->editor(mimeType, this);
            editor->restoreDefaults();
            int index = d->layout->addWidget(editor);
            d->layout->setCurrentIndex(index);
            d->editorHash.insert(id, editor);
        } else {
            d->layout->setCurrentWidget(editor);
        }
        d->setEditor(editor);
        editor->open(url);
    } else {
        QDesktopServices::openUrl(url);
        return;
    }

    d->currentUrl = url;

    emit currentUrlChanged(d->currentUrl);
    emit changed();
}

void Tab::up()
{
    QUrl url = currentUrl();
    QString path = url.path();
    // we can't use QDir::cleanPath because it breaks urls
    // remove / at end of path
    if (path != QLatin1String("/"))
        if (path.endsWith(QLatin1Char('/')))
            path = path.left(path.length() - 1);

    QFileInfo info(path);
    url.setPath(info.path());
    open(url);
}

void Tab::onIndexChanged(int index)
{
    Q_D(Tab);

    HistoryItem item = d->history->itemAt(index);

    if (!item.isValid())
        return;

    d->currentUrl = QUrl(item.path());

    d->ignoreSignals = true;

    int layoutIndex = item.userData(QLatin1String("layoutIndex")).toInt();
    d->layout->setCurrentIndex(layoutIndex);
    AbstractEditor *e = qobject_cast<AbstractEditor *>(d->layout->widget(layoutIndex));
    int historyIndex = item.userData(QLatin1String("index")).toInt();
    if (historyIndex != -1) {
        e->setCurrentIndex(historyIndex);
    } else {
        e->open(QUrl(item.path()));
    }

    d->ignoreSignals = false;
    emit currentUrlChanged(d->currentUrl);
    emit changed();
}

void Tab::onUrlChanged(const QUrl &url)
{
    Q_D(Tab);

    if (d->ignoreSignals)
        return;

    d->addItem(qobject_cast<AbstractEditor*>(sender()), url.toString());
    if (d->currentUrl != url) {
        d->currentUrl = url;

        emit currentUrlChanged(url);
        emit changed();
    }
}
