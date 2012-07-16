#include "editorwindow.h"
#include "editorwindow_p.h"

#include "abstracteditor.h"
#include "actionmanager.h"
#include "ifile.h"
#include "ihistory.h"
#include "commandcontainer.h"
#include "stackedcontainer.h"
#include "history.h"
#include "historybutton.h"
#include "editorwindowfactory.h"
#include "menubarcontainer.h"

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

EditorWindow::EditorWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new EditorWindowPrivate(this))
{
    Q_D(EditorWindow);

    d->editor = 0;
    d->history = new History(this);
    d->createActions();
    d->retranslateUi();
    d->registerActions();

#ifndef Q_OS_MAC
    setMenuBar(ActionManager::instance()->container("MenuBar")->menuBar());
#endif

    connect(d->history, SIGNAL(canGoBackChanged(bool)), d->actions[Back], SLOT(setEnabled(bool)));
    connect(d->history, SIGNAL(canGoForwardChanged(bool)), d->actions[Forward], SLOT(setEnabled(bool)));

    d->menuBarButton = new QToolButton(this);
    d->menuBarButton->setMenu(MenuBarContainer::instance()->menu(d->menuBarButton));
    d->menuBarButton->setPopupMode(QToolButton::InstantPopup);
    d->menuBarButton->setText(tr("Menu"));
    d->menuBarButton->setIcon(QIcon(":/icons/menu.png"));

    QSettings settings;
    settings.beginGroup("MainWindow");
    bool visible = settings.value("menuVisible", true).toBool();
    d->menuVisible = !visible; // to skip check in setMenuVisible
    setMenuVisible(visible);
    d->actions[EditorWindow::ShowMenu]->setChecked(visible);

    d->initGeometry();
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

GuiSystem::AbstractEditor *EditorWindow::editor() const
{
    Q_D(const EditorWindow);

    return d->editor;
}

void EditorWindow::setEditor(AbstractEditor *editor)
{
    Q_D(EditorWindow);

    if (d->editor == editor)
        return;

    if (d->editor) {
        disconnect(d->editor, 0, this, 0);

        if (d->editor->file()) {
            disconnect(d->editor->file(), 0, this, 0);
        }
    }

    d->editor = editor;
    d->history->setHistory(editor->history());

    onWindowIconChanged(d->editor->icon());
    onWindowTitleChanged(d->editor->windowTitle());

    connect(d->editor, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
    connect(d->editor, SIGNAL(openTriggered(QUrl)), SLOT(open(QUrl)));
    connect(d->editor, SIGNAL(iconChanged(QIcon)), SLOT(onWindowIconChanged(QIcon)));
    connect(d->editor, SIGNAL(windowTitleChanged(QString)), SLOT(onWindowTitleChanged(QString)));
    connect(d->editor, SIGNAL(loadStarted()), SLOT(startLoad()));
    connect(d->editor, SIGNAL(loadProgress(int)), SLOT(setLoadProgress(int)));
    connect(d->editor, SIGNAL(loadFinished(bool)), SLOT(finishLoad(bool)));

    if (d->editor->file()) {
        onReadOnlyChanged(d->editor->file()->isReadOnly());
        onModificationChanged(d->editor->file()->isModified());
        connect(d->editor->file(), SIGNAL(modificationChanged(bool)), SLOT(onModificationChanged(bool)));
        connect(d->editor->file(), SIGNAL(readOnlyChanged(bool)), SLOT(onReadOnlyChanged(bool)));
    }

//    bool saveAsEnabled = d->editor->file();
//    bool saveEnabled = saveAsEnabled && !d->editor->file()->isReadOnly() && d->editor->file()->isModified();
//    d->actions[SaveAs]->setEnabled(saveAsEnabled);
//    d->actions[Save]->setEnabled(saveEnabled);
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

    if (d->editor)
        return d->editor->url();

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
    Q_D(EditorWindow);

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

    if (d->editor)
        return d->editor->restoreState(containerState);

    return true;
}

QByteArray EditorWindow::saveState() const
{
    Q_D(const EditorWindow);

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << mainWindowMagic;
    s << mainWindowVersion;
    s << saveGeometry();
    s << QMainWindow::saveState();
    if (d->editor)
        s << d->editor->saveState();

    return state;
}

void EditorWindow::back()
{
    Q_D(EditorWindow);

    if (d->editor)
        d->history->back();
}

void EditorWindow::forward()
{
    Q_D(EditorWindow);

    if (d->editor)
        d->history->forward();
}

void EditorWindow::open(const QUrl &url)
{
    Q_D(const EditorWindow);

    if (d->editor)
        d->editor->open(url);
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

    if (d->editor)
        return;

    if (!d->editor->file())
        return;

    d->editor->file()->save();
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

    if (!d->editor->file())
        return;

    d->editor->file()->save(QUrl::fromLocalFile(path));
}

void EditorWindow::refresh()
{
    Q_D(EditorWindow);

    if (d->editor)
        d->editor->refresh();
}

void EditorWindow::cancel()
{
    Q_D(EditorWindow);

    if (d->editor)
        d->editor->cancel();
}

void EditorWindow::onUrlChanged(const QUrl &/*url*/)
{
}

void EditorWindow::onWindowIconChanged(const QIcon &icon)
{
    QMainWindow::setWindowIcon(icon);
}

void EditorWindow::onWindowTitleChanged(const QString &title)
{
    Q_D(EditorWindow);

    bool modified = d->editor->file()->isModified();
    setWindowTitle(QString("%1%2 - %3").arg(title).arg(modified ? "*" : "").arg(qApp->applicationName()));
}

void EditorWindow::startLoad()
{
}

void EditorWindow::setLoadProgress(int /*progress*/)
{
}

void EditorWindow::finishLoad(bool /*ok*/)
{
}

void EditorWindow::onModificationChanged(bool modified)
{
    Q_D(EditorWindow);

    IFile *file = d->editor ? d->editor->file() : 0;
    bool readOnly = file ? file->isReadOnly() : false;
    d->actions[EditorWindow::Save]->setEnabled(modified && !readOnly);

    onWindowTitleChanged(d->editor->windowTitle());
}

void EditorWindow::onReadOnlyChanged(bool readOnly)
{
    Q_D(EditorWindow);

    d->actions[EditorWindow::SaveAs]->setEnabled(!readOnly);
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

    actions[EditorWindow::Refresh] = new QAction(q);
    QObject::connect(actions[EditorWindow::Refresh], SIGNAL(triggered()), q, SLOT(refresh()));

    actions[EditorWindow::Cancel] = new QAction(q);
    QObject::connect(actions[EditorWindow::Cancel], SIGNAL(triggered()), q, SLOT(cancel()));

    backButton = new HistoryButton;
    backButton->setHistory(history);
    backButton->setDirection(HistoryButton::Back);
    backButton->setIcon(QIcon::fromTheme("go-previous", QIcon(":/images/icons/back.png")));

    forwardButton = new HistoryButton;
    forwardButton->setHistory(history);
    forwardButton->setDirection(HistoryButton::Forward);
    forwardButton->setIcon(QIcon::fromTheme("go-next", QIcon(":/images/icons/forward.png")));

    QWidgetAction *wa;

    wa = new QWidgetAction(q);
    wa->setDefaultWidget(backButton);
    actions[EditorWindow::Back] = wa;
    actions[EditorWindow::Back]->setEnabled(false);
    QObject::connect(actions[EditorWindow::Back], SIGNAL(triggered()), q, SLOT(back()));

    wa = new QWidgetAction(q);
    wa->setDefaultWidget(forwardButton);
    actions[EditorWindow::Forward] = wa;
    actions[EditorWindow::Forward]->setEnabled(false);
    QObject::connect(actions[EditorWindow::Forward], SIGNAL(triggered()), q, SLOT(forward()));

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
    actions[EditorWindow::Refresh]->setText(EditorWindow::tr("Refresh"));
    actions[EditorWindow::Cancel]->setText(EditorWindow::tr("Cancel"));

    backButton->setText(EditorWindow::tr("Back"));
    forwardButton->setText(EditorWindow::tr("Forward"));
}

void EditorWindowPrivate::registerActions()
{
    ActionManager *manager = ActionManager::instance();
    manager->registerAction(actions[EditorWindow::Close], MenuBarContainer::standardCommandName(MenuBarContainer::Close));
    manager->registerAction(actions[EditorWindow::Save], MenuBarContainer::standardCommandName(MenuBarContainer::Save));
    manager->registerAction(actions[EditorWindow::SaveAs], MenuBarContainer::standardCommandName(MenuBarContainer::SaveAs));

    manager->registerAction(actions[EditorWindow::Back], "Back");
    manager->registerAction(actions[EditorWindow::Forward], "Forward");

#ifndef Q_OS_MAC
    manager->registerAction(actions[MainWindow::ShowMenu], MenuBarContainer::standardCommandName(MenuBarContainer::ShowMenu));
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
