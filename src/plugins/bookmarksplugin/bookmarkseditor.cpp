#include "bookmarkseditor.h"

#include "bookmarksconstants.h"

#include <QtCore/QSettings>
#include <QtCore/QUrl>
#include <QtGui/QResizeEvent>
#include <QtGui/QUndoStack>

#include <actionmanager.h>
#include <bookmarksmodel.h>
#include <bookmarkswidget.h>
#include <constants.h>
#include <core.h>
#include <mainwindow.h>
#include <pluginmanager.h>
#include <settings.h>

using namespace Bookmarks;
using namespace BookmarksPlugin;
using namespace CorePlugin;
using namespace GuiSystem;
using namespace ExtensionSystem;

BookmarksEditor::BookmarksEditor(QWidget *parent) :
    AbstractEditor(parent),
    m_widget(new BookmarksWidget(this))
{
    PluginManager *pluginManager = PluginManager::instance();
    m_model = pluginManager->object<BookmarksModel>(QLatin1String(Constants::Objects::BookmarksModel));
    m_widget->setModel(m_model);

    m_settings = Core::instance()->settings();
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

bool BookmarksEditor::open(const QUrl &)
{
    emit currentUrlChanged(currentUrl());
    emit titleChanged(title());
    return true;
}

QUrl BookmarksEditor::currentUrl() const
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

void BookmarksEditor::restoreSession(QSettings &s)
{
    m_widget->restoreState(s.value(QLatin1String("state")).toByteArray());
    AbstractEditor::restoreSession(s);
}

void BookmarksEditor::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("state"), m_widget->saveState());
}

void BookmarksEditor::openTriggered(const QUrl &url)
{
    MainWindow *window = mainWindow();
    if (window)
        window->open(url);
}

void BookmarksEditor::openInTabTriggered(const QUrl &url)
{
    MainWindow *window = mainWindow();
    if (window)
        window->openNewTab(url);
}

void BookmarksEditor::openInWindowTriggered(const QUrl &url)
{
    MainWindow *window = mainWindow();
    if (window)
        window->openNewWindow(url);
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

QStringList BookmarksEditorFactory::mimeTypes()
{
    return QStringList();
}

QByteArray BookmarksEditorFactory::id() const
{
    return Constants::Editors::Bookmarks;
}

AbstractEditor * BookmarksEditorFactory::createEditor(QWidget *parent)
{
    return new BookmarksEditor(parent);
}
