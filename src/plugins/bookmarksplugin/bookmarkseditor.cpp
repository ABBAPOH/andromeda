#include "bookmarkseditor.h"

#include <QtCore/QSettings>

#include <QtGui/QResizeEvent>
#include <QtGui/QUndoStack>

#include <ExtensionSystem/PluginManager>

#include <GuiSystem/ActionManager>
#include <GuiSystem/EditorWindow>
#include <GuiSystem/EditorWindowFactory>
#include <GuiSystem/constants.h>

#include "bookmarksconstants.h"
#include "bookmarksdocument.h"
#include "bookmarksmodel.h"
#include "bookmarksplugin.h"
#include "bookmarkswidget.h"

using namespace Bookmarks;
using namespace GuiSystem;
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

void BookmarksEditor::openTriggered(const QUrl &url)
{
    EditorWindow *window = EditorWindow::currentWindow();
    if (window)
        window->open(url);
}

void BookmarksEditor::openInTabTriggered(const QUrl &url)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->openNewEditor(url);
    }
}

void BookmarksEditor::openInWindowTriggered(const QUrl &url)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        factory->openNewWindow(url);
}

void BookmarksEditor::onStateChanged()
{
    m_settings->setValue(QLatin1String("bookmarksEditor/lastState"), m_widget->saveState());
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

    connect(m_widget, SIGNAL(open(QUrl)), SLOT(openTriggered(QUrl)));
    connect(m_widget, SIGNAL(openInTab(QUrl)), SLOT(openInTabTriggered(QUrl)));
    connect(m_widget, SIGNAL(openInWindow(QUrl)), SLOT(openInWindowTriggered(QUrl)));
    connect(m_widget, SIGNAL(stateChanged()), SLOT(onStateChanged()));

    ActionManager *actionManager = ActionManager::instance();
    redoAction = m_widget->model()->undoStack()->createRedoAction(m_widget);
    m_widget->addAction(redoAction);
    actionManager->registerAction(redoAction, Constants::Actions::Redo);

    undoAction = m_widget->model()->undoStack()->createUndoAction(m_widget);
    m_widget->addAction(undoAction);
    actionManager->registerAction(undoAction, Constants::Actions::Undo);
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
