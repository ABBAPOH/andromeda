#include "stackededitor.h"
#include "stackededitor_p.h"

#include <QtCore/QBuffer>
#include <QtGui/QDesktopServices>
#include <QtGui/QStackedLayout>

using namespace GuiSystem;

void StackedEditorPrivate::setEditor(AbstractEditor *e)
{
    Q_Q(StackedEditor);

    if (editor == e)
        return;

    if (editor) {
        QObject::disconnect(editor, 0, q, 0);
    }

    editor = e;
    QObject::connect(editor, SIGNAL(urlChanged(QUrl)),
                     q, SLOT(onUrlChanged(QUrl)));
    QObject::connect(editor, SIGNAL(openTriggered(QUrl)), q, SLOT(open(QUrl)));
    QObject::connect(editor, SIGNAL(openNewEditorTriggered(QList<QUrl>)),
                     q, SIGNAL(openNewEditorTriggered(QList<QUrl>)));
    QObject::connect(editor, SIGNAL(openNewWindowTriggered(QList<QUrl>)),
                     q, SIGNAL(openNewWindowTriggered(QList<QUrl>)));
    QObject::connect(editor, SIGNAL(iconChanged(QIcon)), q, SIGNAL(iconChanged(QIcon)));
    QObject::connect(editor, SIGNAL(titleChanged(QString)), q, SIGNAL(titleChanged(QString)));
    QObject::connect(editor, SIGNAL(windowTitleChanged(QString)), q, SIGNAL(windowTitleChanged(QString)));
    QObject::connect(editor, SIGNAL(destroyed(QObject*)), q, SLOT(onDestroy(QObject*)));

    QObject::connect(editor, SIGNAL(loadStarted()), q, SIGNAL(loadStarted()));
    QObject::connect(editor, SIGNAL(loadProgress(int)), q, SIGNAL(loadProgress(int)));
    QObject::connect(editor, SIGNAL(loadFinished(bool)), q, SIGNAL(loadFinished(bool)));
}

void StackedEditorPrivate::addItem(AbstractEditor *e)
{
    if (!e)
        return;

    // prevent recursion when navigating through history
    if (e->history()->currentItemIndex() == history->currentItemIndex())
        return;

    HistoryItem item;
    item.setPath(e->url().toString());
    item.setIcon(e->icon());
    item.setLastVisited(QDateTime::currentDateTime());
    item.setTitle(e->title());
    if (e->capabilities() & AbstractEditor::HasHistory)
        item.setUserData(QLatin1String("index"), e->history()->currentItemIndex());
    item.setUserData(QLatin1String("layoutIndex"), layout->indexOf(e));

    history->history()->appendItem(item);
}

/*!
  \class StackedEditor

  \brief The StackedEditor class represents container for editors, layed out in
  a stacked layout.

  Also it implements history of opened urls.
*/

/*!
  \brief Creates a StackedEditor with the given \a parent.
*/
StackedEditor::StackedEditor(QWidget *parent) :
    AbstractEditor(parent),
    d(new StackedEditorPrivate(this))
{
    d->editor = 0;
    d->layout = new QStackedLayout(this);
    d->history = new StackedEditorHistory(this);
    d->ignoreSignals = false;

    connect(d->history, SIGNAL(currentItemIndexChanged(int)), SLOT(onIndexChanged(int)));
}

/*!
  \brief Destroys StackedEditor.
*/
StackedEditor::~StackedEditor()
{
    delete d;
}

/*!
  \reimp
*/
AbstractEditor::Capabilities StackedEditor::capabilities() const
{
    return HasHistory;
}

/*!
  \reimp
*/
QUrl StackedEditor::url() const
{
    return d->currentUrl;
}

/*!
  \reimp
*/
void StackedEditor::open(const QUrl &url)
{
    if (url.isEmpty())
        return;

    if (d->currentUrl == url)
        return;

    d->currentUrl = url;

    EditorManager *manager = EditorManager::instance();
    AbstractEditorFactory *factory = manager->factoryForUrl(url);
    if (factory) {
        QString id = factory->id();
        AbstractEditor *editor = d->editorHash.value(id);
        if (!editor) {
            editor = manager->editorForUrl(url, this);
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

    emit urlChanged(d->currentUrl);
}

/*!
  \reimp
*/
void StackedEditor::close()
{
    if (d->editor)
        d->editor->close();
}

/*!
  \reimp
*/
void StackedEditor::cancel()
{
    if (d->editor)
        d->editor->cancel();
}

/*!
  \reimp
*/
void StackedEditor::refresh()
{
    if (d->editor)
        d->editor->refresh();
}

/*!
  \reimp
*/
QIcon StackedEditor::icon() const
{
    if (d->editor)
        return d->editor->icon();

    return QIcon();
}

/*!
  \reimp
*/
QString StackedEditor::title() const
{
    if (d->editor)
        return d->editor->title();

    return QString();
}

/*!
  \reimp
*/
QString StackedEditor::windowTitle() const
{
    if (d->editor)
        return d->editor->windowTitle();

    return QString();
}

/*!
  \reimp
*/
AbstractHistory * StackedEditor::history() const
{
    return d->history;
}

/*!
  \reimp
*/
void StackedEditor::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QBuffer bufer(&state);
    bufer.open(QBuffer::ReadOnly);
    QDataStream s(&bufer);

    QByteArray id, editorState;
    s >> id;
    s >> editorState;
    AbstractEditor *e = EditorManager::instance()->editorForId(id, this);
    if (e) {
        d->setEditor(e);
        d->layout->addWidget(e);
        d->editorHash.insert(id, e);
        e->restoreState(editorState);
    }
}

/*!
  \reimp
*/
QByteArray StackedEditor::saveState() const
{
    if (!d->editor)
        return  QByteArray();

    QBuffer bufer;
    bufer.open(QBuffer::WriteOnly);
    QDataStream s(&bufer);
    s << d->editor->id();
    s << d->editor->saveState();
    return bufer.data();
}

/*!
  \internal
*/
void StackedEditor::onUrlChanged(const QUrl &url)
{
    if (d->ignoreSignals)
        return;

    d->currentUrl = url;
    d->addItem(qobject_cast<AbstractEditor*>(sender()));
    emit urlChanged(url);
}

/*!
  \internal
*/
void StackedEditor::onIndexChanged(int index)
{
    HistoryItem item = d->history->itemAt(index);

    if (!item.isValid())
        return;

    d->currentUrl = QUrl(item.path());

    d->ignoreSignals = true;

    int layoutIndex = item.userData(QLatin1String("layoutIndex")).toInt();
    d->layout->setCurrentIndex(layoutIndex);
    AbstractEditor *e = qobject_cast<AbstractEditor *>(d->layout->widget(layoutIndex));
    int historyIndex = item.userData(QLatin1String("index")).toInt();
    if (e) {
        if (e->capabilities() & AbstractEditor::HasHistory) {
            e->history()->setCurrentItemIndex(historyIndex);
        } else {
            e->open(QUrl(item.path()));
        }
    }
    d->setEditor(e);

    d->ignoreSignals = false;
    emit urlChanged(d->currentUrl);
}

/*!
  \internal
*/
void StackedEditor::onDestroy(QObject *o)
{
    AbstractEditor *editor = static_cast<AbstractEditor *>(o);

    if (d->editor == editor) {
        d->editor = 0;
    }

    foreach (const QString &key, d->editorHash.keys(editor)) {
        d->editorHash.remove(key);
    }
}
