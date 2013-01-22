#include "editorwindow.h"
#include "editorwindow_p.h"

#include "abstractdocument.h"
#include "abstracteditor.h"
#include "actionmanager.h"
#include "ihistory.h"
#include "commandcontainer.h"
#include "history.h"
#include "historybutton.h"
#include "editorwindowfactory.h"
#include "menubarcontainer.h"
#include "toolmodel.h"
#include "toolwidget.h"
#include "toolwidgetfactory.h"
#include "toolwidgetmanager.h"

#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QSettings>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMenuBar>
#include <QtGui/QWidgetAction>

static const qint32 mainWindowMagic = 0x6d303877; // "m08w"
static const qint8 mainWindowVersion = 1;

using namespace GuiSystem;

/*!
    \class GuiSystem::EditorWindow

    \brief This class is able to display and control specified editor.

    \sa setEditor(AbstractEditor*)
*/
EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new EditorWindowPrivate(this))
{
    Q_D(EditorWindow);

    d->document = 0;
    d->editor = 0;
    d->createActions();
    d->retranslateUi();
    d->registerActions();

#ifndef Q_OS_MAC
    setMenuBar(ActionManager::instance()->container("MenuBar")->menuBar());
#endif

    d->menuBarButton = new QToolButton(this);
    d->menuBarButton->setMenu(MenuBarContainer::instance()->menu(d->menuBarButton));
    d->menuBarButton->setPopupMode(QToolButton::InstantPopup);
    d->menuBarButton->setText(tr("Menu"));
    d->menuBarButton->setIcon(QIcon(":/guisystem/icons/menu.png"));

    QSettings settings;
    settings.beginGroup("MainWindow");
    bool visible = settings.value("menuVisible", true).toBool();
    d->menuVisible = !visible; // to skip check in setMenuVisible
    setMenuVisible(visible);
    d->actions[EditorWindow::ShowMenu]->setChecked(visible);

    d->initGeometry();
    d->createTools();
}

EditorWindow::~EditorWindow()
{
    delete d_ptr;
}

QAction * EditorWindow::action(Action action) const
{
    Q_D(const EditorWindow);

    if (action <= NoAction || action >= ActionCount)
        return 0;

    return d->actions[action];
}

AbstractDocument * EditorWindow::document() const
{
    Q_D(const EditorWindow);
    return d->document;
}

GuiSystem::AbstractEditor *EditorWindow::editor() const
{
    Q_D(const EditorWindow);

    return d->editor;
}

/*!
    \brief Sets current editor and update window state.
*/
void EditorWindow::setEditor(AbstractEditor *editor)
{
    Q_D(EditorWindow);

    if (d->editor == editor)
        return;

    if (d->document)
        disconnect(d->document, 0, this, 0);
    if (d->editor)
        disconnect(d->editor, 0, this, 0);

    d->editor = editor;
    d->document = editor ? editor->document() : 0;

    QList<QDockWidget *> docks = findChildren<QDockWidget *>();
    foreach (QDockWidget *dock, docks) {
        ToolWidget *tool = qobject_cast<ToolWidget *>(dock->widget());
        if (!tool)
            continue;

        tool->setEditor(editor);
    }

    if (d->document) {
        onUrlChanged(d->document->url());
        onWritableChanged(d->document->isWritable());
        onModificationChanged(d->document->isModified());

        onWindowIconChanged(d->document->icon());
        onTitleChanged(d->document->title());

        connect(d->document, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));

        connect(d->document, SIGNAL(iconChanged(QIcon)), SLOT(onWindowIconChanged(QIcon)));
        connect(d->document, SIGNAL(titleChanged(QString)), SLOT(onTitleChanged(QString)));

        connect(d->document, SIGNAL(modificationChanged(bool)), SLOT(onModificationChanged(bool)));
        connect(d->document, SIGNAL(writableChanged(bool)), SLOT(onWritableChanged(bool)));

        connect(d->document, SIGNAL(stateChanged(AbstractDocument::State)), SLOT(onStateChanged(AbstractDocument::State)));
        connect(d->document, SIGNAL(progressChanged(int)), SLOT(onProgressChanged(int)));
    } else {
        onUrlChanged(QUrl());
        onWritableChanged(true);
        onModificationChanged(false);

        onWindowIconChanged(QIcon());
        onTitleChanged(QString());
    }
}

bool EditorWindow::menuVisible() const
{
    Q_D(const EditorWindow);

    return d->menuVisible;
}

void EditorWindow::setMenuVisible(bool visible)
{
    Q_D(EditorWindow);

    if (d->menuVisible == visible)
        return;

    d->menuVisible = visible;

#ifndef Q_OS_MAC
    if (menuBar())
        menuBar()->setVisible(d->menuVisible);
#endif
    d->menuBarButton->setVisible(!d->menuVisible);

    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("menuVisible", visible);

    emit menuVisibleChanged(d->menuVisible);
}

QUrl EditorWindow::url() const
{
    Q_D(const EditorWindow);

    if (d->document)
        return d->document->url();

    return QUrl();
}

QToolButton *EditorWindow::menuBarButton() const
{
    Q_D(const EditorWindow);

    return d->menuBarButton;
}

EditorWindow * EditorWindow::currentWindow()
{
    return qobject_cast<EditorWindow*>(qApp->activeWindow());
}

QList<EditorWindow *> EditorWindow::windows()
{
    QList<EditorWindow*> result;
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        EditorWindow* window = qobject_cast<EditorWindow*>(widget);
        if (window)
            result.append(window);
    }
    return result;
}

EditorWindow * EditorWindow::createWindow()
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory)
        return factory->create();
    else
        qWarning() << "MainWindow::createWindow:" << "MainWindowFactory is not set";

    return 0;
}

bool EditorWindow::restoreState(const QByteArray &arr)
{
    QByteArray state = arr;

    QDataStream s(&state, QIODevice::ReadOnly);

    qint32 magic;
    qint8 version;
    QByteArray geometry;
    QByteArray mainWindowState;
    QByteArray containerState;

    s >> magic;
    if (magic != mainWindowMagic)
        return false;

    s >> version;
    if (version != mainWindowVersion)
        return false;

    s >> geometry;
    s >> mainWindowState;
    s >> containerState;
    restoreGeometry(geometry);
    QMainWindow::restoreState(mainWindowState);

    return true;
}

QByteArray EditorWindow::saveState() const
{
    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << mainWindowMagic;
    s << mainWindowVersion;
    s << saveGeometry();
    s << QMainWindow::saveState();

    return state;
}

void EditorWindow::open(const QUrl &url)
{
    Q_D(const EditorWindow);

    if (d->document)
        d->document->setUrl(url);
}

void EditorWindow::close()
{
    QMainWindow::close();
}

void EditorWindow::openNewWindow(const QUrl &url)
{
    EditorWindowFactory *factory = EditorWindowFactory::defaultFactory();
    if (factory) {
        factory->openNewWindow(url);
    } else {
        qWarning() << "MainWindow::openNewWindow :" << "Must call MainWindowFactory::setDefaultfactory first";
    }
}

//void MainWindow::openNewWindow(const QList<QUrl> &urls)
//{
//    MainWindowFactory *factory = MainWindowFactory::defaultFactory();
//    if (factory) {
//            factory->open(MainWindowFactory::OpenInNewWindow, urls);
//    } else {
//        qWarning() << "MainWindow::openNewWindow :" << "Must call MainWindowFactory::setDefaultfactory first";
//    }
//}

void EditorWindow::save()
{
    Q_D(EditorWindow);

    if (!d->editor)
        return;

    if (!d->editor->document())
        return;

    if (d->document->url().isEmpty() || !d->document->url().isValid())
        return;

    d->document->save(d->document->url());
}

void EditorWindow::saveAs()
{
    Q_D(EditorWindow);

    if (!d->editor)
        return;

    // TODO: remember previous dir and set filename using title + extension from mimetype
    QString path = QFileDialog::getSaveFileName(this, tr("Save as"));

    if (path.isEmpty())
        return;

    if (!d->editor->document())
        return;

    d->editor->document()->save(QUrl::fromLocalFile(path));
}

void EditorWindow::reload()
{
    Q_D(EditorWindow);

    if (d->document)
        d->document->reload();
}

void EditorWindow::stop()
{
    Q_D(EditorWindow);

    if (d->document)
        d->document->stop();
}

void EditorWindow::onUrlChanged(const QUrl &/*url*/)
{
}

void EditorWindow::onWindowIconChanged(const QIcon &icon)
{
    QMainWindow::setWindowIcon(icon);
}

void EditorWindow::onTitleChanged(const QString &title)
{
    Q_D(EditorWindow);

    bool modified = d->document ? d->document->isModified() : false;
    setWindowTitle(QString("%1%2 - %3").arg(title).arg(modified ? "*" : "").arg(qApp->applicationName()));
}

void EditorWindow::onProgressChanged(int /*progress*/)
{
}

void EditorWindow::onStateChanged(AbstractDocument::State /*state*/)
{
}

void EditorWindow::onModificationChanged(bool modified)
{
    Q_D(EditorWindow);

    bool writable = d->document ? d->document->isWritable() : false;
    d->actions[EditorWindow::Save]->setEnabled(modified && !writable);

    onTitleChanged(d->document ? d->document->title() : QString());
}

void EditorWindow::onWritableChanged(bool writable)
{
    Q_D(EditorWindow);

    bool modified = d->document ? d->document->isModified() : false;
    d->actions[EditorWindow::Save]->setEnabled(modified && !writable);

//    d->actions[EditorWindow::SaveAs]->setEnabled(!modified);
}

void EditorWindowPrivate::createActions()
{
    Q_Q(EditorWindow);

    actions[EditorWindow::OpenFile] = new QAction(q);

    actions[EditorWindow::Close] = new QAction(q);
    QObject::connect(actions[EditorWindow::Close], SIGNAL(triggered()), q, SLOT(close()));

    actions[EditorWindow::Save] = new QAction(q);
    QObject::connect(actions[EditorWindow::Save], SIGNAL(triggered()), q, SLOT(save()));

    actions[EditorWindow::SaveAs] = new QAction(q);
    QObject::connect(actions[EditorWindow::SaveAs], SIGNAL(triggered()), q, SLOT(saveAs()));

    actions[EditorWindow::Reload] = new QAction(q);
    QObject::connect(actions[EditorWindow::Reload], SIGNAL(triggered()), q, SLOT(reload()));

    actions[EditorWindow::Stop] = new QAction(q);
    QObject::connect(actions[EditorWindow::Stop], SIGNAL(triggered()), q, SLOT(stop()));

    actions[EditorWindow::ShowMenu] = new QAction(q);
    actions[EditorWindow::ShowMenu]->setCheckable(true);
    actions[EditorWindow::ShowMenu]->setChecked(true);
    QObject::connect(actions[EditorWindow::ShowMenu], SIGNAL(triggered(bool)), q, SLOT(setMenuVisible(bool)));

    for (int i = 0; i < EditorWindow::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void EditorWindowPrivate::retranslateUi()
{
    actions[EditorWindow::Save]->setText(EditorWindow::tr("Save"));
    actions[EditorWindow::SaveAs]->setText(EditorWindow::tr("Save as..."));
    actions[EditorWindow::Reload]->setText(EditorWindow::tr("Refresh"));
    actions[EditorWindow::Stop]->setText(EditorWindow::tr("Cancel"));
}

void EditorWindowPrivate::registerActions()
{
    ActionManager *manager = ActionManager::instance();
    manager->registerAction(actions[EditorWindow::Close], MenuBarContainer::standardCommandName(MenuBarContainer::Close));
    manager->registerAction(actions[EditorWindow::Save], MenuBarContainer::standardCommandName(MenuBarContainer::Save));
    manager->registerAction(actions[EditorWindow::SaveAs], MenuBarContainer::standardCommandName(MenuBarContainer::SaveAs));

#ifndef Q_OS_MAC
    manager->registerAction(actions[EditorWindow::ShowMenu], MenuBarContainer::standardCommandName(MenuBarContainer::ShowMenu));
#endif
}

void EditorWindowPrivate::initGeometry()
{
    Q_Q(EditorWindow);

    static const float percent = 0.58f;
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry(q);
    QSize desktopSize = QSize(desktopRect.width(), desktopRect.height());
    q->setGeometry(desktopRect.x() + desktopSize.width()*(1.0 - percent)/2.0,
                   desktopRect.y() + desktopSize.height()*(1.0 - percent)/3.0,
                   desktopSize.width()*percent,
                   desktopSize.height()*percent);
}

void EditorWindowPrivate::createTools()
{
    Q_Q(EditorWindow);


    QList<ToolWidgetFactory *> factories = ToolWidgetManager::instance()->factories();
    auto factoriesLessThan = [=](ToolWidgetFactory *a, ToolWidgetFactory *b)
    {
        return a->defaultArea() < b->defaultArea();
    };
    qStableSort(factories.begin(), factories.end(), factoriesLessThan);

    for (int i = 0; i < factories.count(); ++i) {
        ToolWidgetFactory *factory = factories.at(i);
        QDockWidget *dock = createTool(factory);
        QAction *action = dock->toggleViewAction();
        action->setShortcut(QString("Meta+%1").arg(i + 1));
        q->addAction(action);
    }
}

QDockWidget * EditorWindowPrivate::createTool(ToolWidgetFactory *factory)
{
    Q_Q(EditorWindow);

    DockWidget *dock = new DockWidget(q);
    dock->setObjectName(factory->id());
    ToolWidget *tool = factory->createToolWidget(dock);
    Q_ASSERT(tool);
    dock->setWidget(tool);
    dock->setWindowTitle(tool->model()->title());
    dock->hide();
    q->connect(tool->model(), SIGNAL(titleChanged(QString)), dock, SLOT(setWindowTitle(QString)));
    q->addDockWidget(Qt::DockWidgetArea(factory->defaultArea()), dock);
    return dock;
}

EditorWindowPrivate::DockWidget::DockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    TabBar *tabBar = new TabBar(this);
    tabBar->setDocumentMode(true);
    tabBar->addTab(windowTitle());
    tabBar->setTabsClosable(true);
    connect(tabBar, SIGNAL(tabCloseRequested(int)), this, SLOT(close()));
    setTitleBarWidget(tabBar);
}

void EditorWindowPrivate::DockWidget::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowTitleChange) {
        TabBar *tabBar = qobject_cast<TabBar *>(titleBarWidget());
        tabBar->setTabText(0, windowTitle());
    }
    QDockWidget::changeEvent(event);
}

EditorWindowPrivate::DockWidget::TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
}

QSize EditorWindowPrivate::DockWidget::TabBar::minimumSizeHint() const
{
    return QSize();
}

QSize EditorWindowPrivate::DockWidget::TabBar::tabSizeHint(int index) const
{
    QSize s = QTabBar::tabSizeHint(index);
    if (index == 0)
        s.setWidth(this->width());
    else
        s.setWidth(0);
    return s;
}
