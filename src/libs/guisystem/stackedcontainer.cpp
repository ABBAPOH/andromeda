#include "stackedcontainer.h"
#include "stackedcontainer_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <QtGui/QDesktopServices>
#include <QtGui/QStackedLayout>

#include "stackedhistory.h"

using namespace GuiSystem;

void StackedContainerPrivate::openEditor(const QUrl &url, AbstractEditorFactory *factory)
{
    Q_Q(StackedContainer);

    if (factory) {
        QString id = factory->id();
        AbstractEditor *editor = editorHash.value(id);
        if (!editor) {
            editor = factory->editor(q);
            editor->restoreDefaults();
            int index = layout->addWidget(editor);
            layout->setCurrentIndex(index);
            editorHash.insert(id, editor);
        } else {
            layout->setCurrentWidget(editor);
        }
        q->setSourceEditor(editor);
        stackedHistory->open(url);
        editor->open(url);
    } else {
        QDesktopServices::openUrl(url);
        return;
    }
}

/*!
  \class GuiSystem::StackedEditor

  \internal
  \brief The StackedEditor class represents container for editors, layed out in
  a stacked layout.

  Also it implements history of opened urls.
*/

/*!
  \brief Creates a StackedEditor with the given \a parent.
*/
StackedContainer::StackedContainer(QWidget *parent) :
    ProxyEditor(parent),
    d(new StackedContainerPrivate(this))
{
    d->editor = 0;
    d->layout = new QStackedLayout(this);
    d->file = new ProxyFile(this);
    d->ignoreSignals = false;

    d->stackedHistory = new StackedHistory(this);
    d->stackedHistory->setContainer(this);

    connect(d->layout, SIGNAL(currentChanged(int)), SIGNAL(currentChanged(int)));

    new StackedHistory;
}

/*!
  \brief Destroys StackedEditor.
*/
StackedContainer::~StackedContainer()
{
    delete d;
}

void StackedContainer::setSourceEditor(AbstractEditor *editor)
{
    if (d->editor == editor)
        return;

    if (d->editor) {
        QObject::disconnect(d->editor, SIGNAL(urlChanged(QUrl)),
                            this, SLOT(onUrlChanged(QUrl)));
        QObject::disconnect(d->editor, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroy(QObject*)));
    }

    d->editor = editor;
    ProxyEditor::setSourceEditor(editor);
    d->file->setSourceFile(editor->file());
    connect(editor, SIGNAL(urlChanged(QUrl)),
                     this, SLOT(onUrlChanged(QUrl)));
    connect(editor, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroy(QObject*)));
}

AbstractEditor *StackedContainer::currentEditor() const
{
    return d->editor;
}

/*!
  \reimp
*/
IFile * StackedContainer::file() const
{
    return d->file;
}

/*!
  \reimp
*/
QUrl StackedContainer::url() const
{
    return d->currentUrl;
}

/*!
  \reimp
*/
void StackedContainer::open(const QUrl &dirtyUrl)
{
    QUrl url = dirtyUrl;
    url.setPath(QDir::cleanPath(url.path()));

    if (url.isEmpty())
        return;

    if (d->currentUrl == url)
        return;

    d->currentUrl = url;

    EditorManager *manager = EditorManager::instance();
    AbstractEditorFactory *factory = manager->factoryForUrl(url);
    d->openEditor(url, factory);

    // todo: remove?
    emit urlChanged(d->currentUrl);
}

void StackedContainer::openEditor(const QUrl &dirtyUrl, const QByteArray &editor)
{
    QUrl url = dirtyUrl;
    url.setPath(QDir::cleanPath(url.path()));

    if (url.isEmpty())
        return;

    if (d->currentUrl == url)
        return;

    d->currentUrl = url;

    EditorManager *manager = EditorManager::instance();
    AbstractEditorFactory *factory = manager->factoryForId(editor);
    d->openEditor(url, factory);

    // todo: remove?
    emit urlChanged(d->currentUrl);
}

/*!
  \reimp
*/
IHistory * StackedContainer::history() const
{
    return d->stackedHistory;
}

/*!
  \reimp
*/
bool StackedContainer::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;
    QDataStream s(&state, QIODevice::ReadOnly);

    QByteArray id, editorState;
    s >> id;
    s >> editorState;

    AbstractEditorFactory *factory = EditorManager::instance()->factoryForId(id);
    if (!factory)
        return false;

    AbstractEditor *editor = factory->editor(this);
    if (!editor)
        return false;

    setSourceEditor(editor);
    d->layout->addWidget(editor);
    d->editorHash.insert(id, editor);
    d->stackedHistory->open(QUrl());
    return editor->restoreState(editorState);
}

/*!
  \reimp
*/
QByteArray StackedContainer::saveState() const
{
    if (!d->editor)
        return  QByteArray();

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);
    s << d->editor->id();
    s << d->editor->saveState();
    return state;
}

AbstractEditor *StackedContainer::editor(const QString &id) const
{
    return d->editorHash.value(id);
}

bool StackedContainer::setEditor(const QString &id)
{
    AbstractEditor * editor = d->editorHash.value(id);
    if (!editor) {
        qWarning() << "Can't open editor with id" << id;
        return false;
    }

    d->layout->setCurrentWidget(editor);
    setSourceEditor(editor);

    d->currentUrl = d->editor->url();

    emit urlChanged(d->currentUrl);
    emit iconChanged(d->editor->icon());
    emit titleChanged(d->editor->title());

    return true;
}

/*!
  \internal
*/
void StackedContainer::onUrlChanged(const QUrl &url)
{
    if (d->ignoreSignals)
        return;

    d->currentUrl = url;
    emit urlChanged(url);
}

/*!
  \internal
*/
void StackedContainer::onDestroy(QObject *o)
{
     AbstractEditor *editor = static_cast<AbstractEditor *>(o);

    if (d->editor == editor) {
        d->editor = 0;
    }

    foreach (const QString &key, d->editorHash.keys(editor)) {
        d->editorHash.remove(key);
    }
}
