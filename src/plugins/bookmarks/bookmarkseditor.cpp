#include "bookmarkseditor.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>

#include <QtGui/QResizeEvent>
#include <QtGui/QUndoStack>

#include <extensionsystem/pluginmanager.h>
#include <guisystem/actionmanager.h>
#include <guisystem/editorwindow.h>
#include <guisystem/editorwindowfactory.h>

#include <core/constants.h>

#include "bookmarksconstants.h"
#include "bookmarksmodel.h"
#include "bookmarkswidget.h"

using namespace Bookmarks;
using namespace GuiSystem;
using namespace ExtensionSystem;

BookmarksEditor::BookmarksEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_widget(new BookmarksWidget(this))
{
    PluginManager *pluginManager = PluginManager::instance();
    m_model = pluginManager->object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));
    m_widget->setModel(m_model);

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

void BookmarksEditor::open(const QUrl &)
{
    emit urlChanged(url());
    emit titleChanged(title());
    emit iconChanged(icon());
    emit windowTitleChanged(windowTitle());
}

QUrl BookmarksEditor::url() const
{
    return QUrl(QLatin1String("andromeda://bookmarks"));
}

QIcon BookmarksEditor::icon() const
{
    return QIcon(":/icons/bookmarks.png");
}

QString BookmarksEditor::title() const
{
    return tr("Bookmarks");
}

QString BookmarksEditor::windowTitle() const
{
    return tr("Bookmarks");
}

QByteArray BookmarksEditor::saveState() const
{
    return m_widget->saveState();
}

bool BookmarksEditor::restoreState(const QByteArray &state)
{
    m_widget->restoreState(state);
    open(url());
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

BookmarksEditorFactory::BookmarksEditorFactory(QObject *parent) :
    AbstractEditorFactory(parent)
{
}

QByteArray BookmarksEditorFactory::id() const
{
    return Constants::Editors::Bookmarks;
}

QString BookmarksEditorFactory::name() const
{
    return tr("Bookmarks editor");
}

QIcon BookmarksEditorFactory::icon() const
{
    return QIcon(":/icons/bookmarks.png");
}

AbstractEditor * BookmarksEditorFactory::createEditor(QWidget *parent)
{
    return new BookmarksEditor(parent);
}
