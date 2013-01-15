#include "editorview.h"
#include "editorview_p.h"

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QDir>

#include <QtGui/QAction>
#include <QtGui/QDesktopServices>
#include <QtGui/QStackedLayout>

#include "abstractdocument.h"
#include "abstractdocumentfactory.h"
#include "constants.h"
#include "documentmanager.h"
#include "editorviewhistory.h"
#include "editormanager.h"
#include "findtoolbar.h"

using namespace GuiSystem;

void EditorViewPrivate::init()
{
    Q_Q(EditorView);

    editor = 0;

    mainLayout = new QVBoxLayout(q);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    findToolBar = new FindToolBar(q);
    findToolBar->hide();
    mainLayout->addWidget(findToolBar);

    layout = new QStackedLayout(mainLayout);
    ignoreSignals = false;

    stackedHistory = new EditorViewHistory(q);
    stackedHistory->setContainer(q);

    QObject::connect(layout, SIGNAL(currentChanged(int)), q, SIGNAL(editorChanged()));

    findAction = new QAction(EditorView::tr("Find"), q);
    findAction->setObjectName(Constants::Actions::Find);
    QObject::connect(findAction, SIGNAL(triggered()), findToolBar, SLOT(openFind()));
    q->addAction(findAction);
}

void EditorViewPrivate::openEditor(const QUrl &url, AbstractDocumentFactory *factory)
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
    d->init();
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

    d->findToolBar->hide();
    IFind *find = editor ? editor->find() : 0;
    d->findToolBar->setFind(find);
    d->findAction->setEnabled(find);

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

    DocumentManager *manager = DocumentManager::instance();
    AbstractDocumentFactory *factory = manager->factoryForUrl(url);
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

    DocumentManager *manager = DocumentManager::instance();
    AbstractDocumentFactory *factory = manager->factoryForId(editor);
    d->openEditor(url, factory);
}

void EditorView::openEditor(const QByteArray &editorId)
{
    if (d->editor) {
        QByteArray oldId = d->editor->property("id").toByteArray();
        Q_ASSERT(!oldId.isEmpty());
        if (oldId == editorId) // ok, we're already opened
            return;
    }

    AbstractEditor *oldEditor = d->editor;
    AbstractEditor *editor = EditorManager::instance()->createEditor(editorId, this);
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

    AbstractEditor *editor = EditorManager::instance()->createEditor(id, this);
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

    QByteArray id = d->editor->property("id").toByteArray();
    Q_ASSERT_X(!id.isEmpty(), "EditorView::saveState",
               "Editor has no id, that may mean it was created without factory");
    s << id;
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
