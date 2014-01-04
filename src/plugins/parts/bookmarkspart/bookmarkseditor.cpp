#include "bookmarkseditor.h"

#include <QtCore/QSettings>

#include <QtGui/QResizeEvent>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QUndoStack>
#else
#include <QtGui/QAction>
#include <QtGui/QUndoStack>
#endif

#include <ExtensionSystem/PluginManager>

#include <Parts/EditorWindow>
#include <Parts/EditorWindowFactory>
#include <Parts/constants.h>

#include <Bookmarks/BookmarksModel>
#include <Bookmarks/BookmarksWidget>

#include "bookmarksconstants.h"
#include "bookmarksdocument.h"
#include "bookmarksplugin.h"

using namespace Bookmarks;
using namespace Parts;
using namespace ExtensionSystem;

BookmarksEditor::BookmarksEditor(QWidget *parent) :
    AbstractEditor(*new BookmarksDocument, parent),
    m_widget(new BookmarksWidget(this))
{
    document()->setParent(this);
    init();
}

BookmarksEditor::BookmarksEditor(BookmarksDocument &document, QWidget *parent) :
    AbstractEditor(document, parent),
    m_widget(new BookmarksWidget(this))
{
    init();
}

QByteArray BookmarksEditor::saveState() const
{
    return m_widget->saveState();
}

bool BookmarksEditor::restoreState(const QByteArray &state)
{
    m_widget->restoreState(state);
    return true;
}

void BookmarksEditor::openTriggered(const QList<QUrl> &urls)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (!factory)
        return;
    factory->open(urls);
}

void BookmarksEditor::resizeEvent(QResizeEvent *e)
{
    m_widget->resize(e->size());
}

void BookmarksEditor::init()
{
    BookmarksDocument *doc = qobject_cast<BookmarksDocument *>(document());
    Q_ASSERT(doc);
    m_widget->setModel(doc->model());

    m_settings = new QSettings(this);
    QVariant value = m_settings->value(QLatin1String("bookmarksEditor/lastState"));
    if (value.isValid())
        m_widget->restoreState(value.toByteArray());

    connect(m_widget, SIGNAL(openRequested(QList<QUrl>)), SLOT(openTriggered(QList<QUrl>)));

    redoAction = m_widget->model()->undoStack()->createRedoAction(m_widget);
    redoAction->setObjectName(Constants::Actions::Redo);
    m_widget->addAction(redoAction);

    undoAction = m_widget->model()->undoStack()->createUndoAction(m_widget);
    undoAction->setObjectName(Constants::Actions::Undo);
    m_widget->addAction(undoAction);
}

/*!
    \class BookmarksEditorFactory
*/
BookmarksEditorFactory::BookmarksEditorFactory(QObject *parent) :
    AbstractEditorFactory(Constants::Editors::Bookmarks, parent)
{
}

/*!
    \reimp
*/
AbstractEditor * BookmarksEditorFactory::createEditor(QWidget *parent)
{
    return new BookmarksEditor(*BookmarksPlugin::instance()->sharedDocument(), parent);
}
