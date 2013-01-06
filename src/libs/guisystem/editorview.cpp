#include "editorview.h"
#include "editorview_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <QtGui/QDesktopServices>
#include <QtGui/QStackedLayout>

#include "abstractdocument.h"
#include "editorviewhistory.h"

using namespace GuiSystem;

void EditorViewPrivate::openEditor(const QUrl &url, AbstractEditorFactory *factory)
{
    Q_Q(EditorView);

    if (factory) {
        AbstractEditor *oldEditor = this->editor;

        q->openEditor(factory->id());
        stackedHistory->open(url, oldEditor);
        editor->document()->setUrl(url);
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
EditorView::EditorView(QWidget *parent) :
    QWidget(parent),
    d(new EditorViewPrivate(this))
{
    d->editor = 0;
    d->layout = new QStackedLayout(this);
    d->ignoreSignals = false;

    d->stackedHistory = new EditorViewHistory(this);
    d->stackedHistory->setContainer(this);

    connect(d->layout, SIGNAL(currentChanged(int)), SIGNAL(editorChanged()));

    new EditorViewHistory;
}

/*!
  \brief Destroys StackedEditor.
*/
EditorView::~EditorView()
{
    delete d;
}

void EditorView::setSourceEditor(AbstractEditor *editor)
{
    if (d->editor == editor)
        return;

    if (d->editor) {
        QObject::disconnect(d->document, SIGNAL(urlChanged(QUrl)),
                            this, SLOT(onUrlChanged(QUrl)));
    }

    d->editor = editor;
    d->document = editor ? editor->document() : 0;
    if (d->document) {
        connect(d->document, SIGNAL(urlChanged(QUrl)),
                this, SLOT(onUrlChanged(QUrl)));
    }

    emit editorChanged();
}

AbstractEditor *EditorView::currentEditor() const
{
    return d->editor;
}

/*!
  \reimp
*/
QUrl EditorView::url() const
{
    return d->currentUrl;
}

/*!
  \reimp
*/
void EditorView::open(const QUrl &dirtyUrl)
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
}

void EditorView::openEditor(const QUrl &dirtyUrl, const QByteArray &editor)
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
}

void EditorView::openEditor(const QByteArray &editorId)
{
    EditorManager *manager = EditorManager::instance();
    AbstractEditorFactory *factory = manager->factoryForId(editorId);
    AbstractEditor *oldEditor = d->editor;
    AbstractEditor *editor = factory->editor(this);
    editor->restoreDefaults();
    int index = d->layout->addWidget(editor);
    d->layout->setCurrentIndex(index);
    setSourceEditor(editor);
    if (oldEditor)
        oldEditor->deleteLater();
}

/*!
  \reimp
*/
IHistory * EditorView::history() const
{
    return d->stackedHistory;
}

/*!
  \reimp
*/
bool EditorView::restoreState(const QByteArray &arr)
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
//    d->editorHash.insert(id, editor);
    d->stackedHistory->open(QUrl(), 0);
    return editor->restoreState(editorState);
}

/*!
  \reimp
*/
QByteArray EditorView::saveState() const
{
    if (!d->editor)
        return  QByteArray();

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);
    s << d->editor->property("id").toByteArray();
    s << d->editor->saveState();
    return state;
}

AbstractDocument *EditorView::document() const
{
    if (!d->editor)
        return 0;

    return d->editor->document();
}

AbstractEditor *EditorView::editor() const
{
    return d->editor;
}

/*!
  \internal
*/
void EditorView::onUrlChanged(const QUrl &url)
{
    if (d->ignoreSignals)
        return;

    d->currentUrl = url;
}
