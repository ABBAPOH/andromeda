#include "mainwindow.h"
#include "mainwindow_p.h"

#include "abstracteditor.h"
#include "actionmanager.h"
#include "ifile.h"
#include "ihistory.h"
#include "commandcontainer.h"
#include "stackedcontainer.h"
#include "history.h"
#include "historybutton.h"
#include "mainwindowfactory.h"
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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

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
    d->actions[MainWindow::ShowMenu]->setChecked(visible);

    d->initGeometry();
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

QAction * MainWindow::action(Action action) const
{
    Q_D(const MainWindow);

    if (action <= NoAction || action >= ActionCount)
        return 0;

    return d->actions[action];
}

GuiSystem::AbstractEditor *MainWindow::editor() const
{
    Q_D(const MainWindow);

    return d->editor;
}

void MainWindow::setEditor(AbstractEditor *editor)
{
    Q_D(MainWindow);

    if (d->editor == editor)
        return;

    if (d->editor) {
        disconnect(d->editor, 0, this, 0);

        if (d->editor->file())
        disconnect(d->editor->file(), 0, d->actions[Save], 0);
    }

    d->editor = editor;
    d->history->setHistory(editor->history());

    connect(d->editor, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
    connect(d->editor, SIGNAL(openTriggered(QUrl)), SLOT(open(QUrl)));
    connect(d->editor, SIGNAL(iconChanged(QIcon)), SLOT(setWindowIcon(QIcon)));
    connect(d->editor, SIGNAL(windowTitleChanged(QString)), SLOT(setWindowTitle(QString)));
    connect(d->editor, SIGNAL(windowTitleChanged(QString)), SLOT(setWindowTitle(QString)));
    connect(d->editor, SIGNAL(loadStarted()), SLOT(startLoad()));
    connect(d->editor, SIGNAL(loadProgress(int)), SLOT(setLoadProgress(int)));
    connect(d->editor, SIGNAL(loadFinished(bool)), SLOT(finishLoad(bool)));

    if (d->editor->file())
        connect(d->editor->file(), SIGNAL(modificationChanged(bool)), d->actions[Save], SLOT(setEnabled(bool)));

    bool saveAsEnabled = d->editor->file();
    bool saveEnabled = saveAsEnabled && !d->editor->file()->isReadOnly() && d->editor->file()->isModified();
    d->actions[SaveAs]->setEnabled(saveAsEnabled);
    d->actions[Save]->setEnabled(saveEnabled);

    setCentralWidget(d->editor);
}

bool MainWindow::menuVisible() const
{
    Q_D(const MainWindow);

    return d->menuVisible;
}

void MainWindow::setMenuVisible(bool visible)
{
    Q_D(MainWindow);

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

QUrl MainWindow::url() const
{
    Q_D(const MainWindow);

    if (d->editor)
        return d->editor->url();

    return QUrl();
}

QToolButton *MainWindow::menuBarButton() const
{
    Q_D(const MainWindow);

    return d->menuBarButton;
}

MainWindow * MainWindow::currentWindow()
{
    return qobject_cast<MainWindow*>(qApp->activeWindow());
}

QList<MainWindow *> MainWindow::windows()
{
    QList<MainWindow*> result;
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        MainWindow* window = qobject_cast<MainWindow*>(widget);
        if (window)
            result.append(window);
    }
    return result;
}

MainWindow * MainWindow::createWindow()
{
    MainWindowFactory *factory = MainWindowFactory::defaultFactory();
    if (factory)
        return factory->create();
    else
        qWarning() << "MainWindow::createWindow:" << "MainWindowFactory is not set";

    return 0;
}

bool MainWindow::restoreState(const QByteArray &arr)
{
    Q_D(MainWindow);

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

QByteArray MainWindow::saveState() const
{
    Q_D(const MainWindow);

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

void MainWindow::back()
{
    Q_D(MainWindow);

    if (d->editor)
        d->history->back();
}

void MainWindow::forward()
{
    Q_D(MainWindow);

    if (d->editor)
        d->history->forward();
}

void MainWindow::open(const QUrl &url)
{
    Q_D(const MainWindow);

    if (d->editor)
        d->editor->open(url);
}

void MainWindow::close()
{
    QMainWindow::close();
}

void MainWindow::openNewWindow(const QUrl &url)
{
    MainWindowFactory *factory = MainWindowFactory::defaultFactory();
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

void MainWindow::save()
{
    Q_D(MainWindow);

    if (d->editor)
        return;

    if (!d->editor->file())
        return;

    d->editor->file()->save();
}

void MainWindow::saveAs()
{
    Q_D(MainWindow);

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

void MainWindow::refresh()
{
    Q_D(MainWindow);

    if (d->editor)
        d->editor->refresh();
}

void MainWindow::cancel()
{
    Q_D(MainWindow);

    if (d->editor)
        d->editor->cancel();
}

void MainWindow::setWindowIcon(const QIcon &icon)
{
    QMainWindow::setWindowIcon(icon);
}

void MainWindow::onUrlChanged(const QUrl &/*url*/)
{
}

void MainWindow::startLoad()
{
}

void MainWindow::setLoadProgress(int /*progress*/)
{
}

void MainWindow::finishLoad(bool /*ok*/)
{
}

void MainWindowPrivate::createActions()
{
    Q_Q(MainWindow);

    actions[MainWindow::OpenFile] = new QAction(q);

    actions[MainWindow::Close] = new QAction(q);
    QObject::connect(actions[MainWindow::Close], SIGNAL(triggered()), q, SLOT(close()));

    actions[MainWindow::Save] = new QAction(q);
    QObject::connect(actions[MainWindow::Save], SIGNAL(triggered()), q, SLOT(save()));

    actions[MainWindow::SaveAs] = new QAction(q);
    QObject::connect(actions[MainWindow::SaveAs], SIGNAL(triggered()), q, SLOT(saveAs()));

    actions[MainWindow::Refresh] = new QAction(q);
    QObject::connect(actions[MainWindow::Refresh], SIGNAL(triggered()), q, SLOT(refresh()));

    actions[MainWindow::Cancel] = new QAction(q);
    QObject::connect(actions[MainWindow::Cancel], SIGNAL(triggered()), q, SLOT(cancel()));

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
    actions[MainWindow::Back] = wa;
    actions[MainWindow::Back]->setEnabled(false);
    QObject::connect(actions[MainWindow::Back], SIGNAL(triggered()), q, SLOT(back()));

    wa = new QWidgetAction(q);
    wa->setDefaultWidget(forwardButton);
    actions[MainWindow::Forward] = wa;
    actions[MainWindow::Forward]->setEnabled(false);
    QObject::connect(actions[MainWindow::Forward], SIGNAL(triggered()), q, SLOT(forward()));

    actions[MainWindow::ShowMenu] = new QAction(q);
    actions[MainWindow::ShowMenu]->setCheckable(true);
    actions[MainWindow::ShowMenu]->setChecked(true);
    QObject::connect(actions[MainWindow::ShowMenu], SIGNAL(triggered(bool)), q, SLOT(setMenuVisible(bool)));

    for (int i = 0; i < MainWindow::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void MainWindowPrivate::retranslateUi()
{
    actions[MainWindow::Save]->setText(MainWindow::tr("Save"));
    actions[MainWindow::SaveAs]->setText(MainWindow::tr("Save as..."));
    actions[MainWindow::Refresh]->setText(MainWindow::tr("Refresh"));
    actions[MainWindow::Cancel]->setText(MainWindow::tr("Cancel"));

    backButton->setText(MainWindow::tr("Back"));
    forwardButton->setText(MainWindow::tr("Forward"));
}

void MainWindowPrivate::registerActions()
{
    ActionManager *manager = ActionManager::instance();
    manager->registerAction(actions[MainWindow::Close], MenuBarContainer::standardCommandName(MenuBarContainer::Close));

    manager->registerAction(actions[MainWindow::Back], "Back");
    manager->registerAction(actions[MainWindow::Forward], "Forward");

#ifndef Q_OS_MAC
    manager->registerAction(actions[MainWindow::ShowMenu], MenuBarContainer::standardCommandName(MenuBarContainer::ShowMenu));
#endif
}

void MainWindowPrivate::initGeometry()
{
    Q_Q(MainWindow);

    static const float percent = 0.58f;
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry(q);
    QSize desktopSize = QSize(desktopRect.width(), desktopRect.height());
    q->setGeometry(desktopRect.x() + desktopSize.width()*(1.0 - percent)/2.0,
                   desktopRect.y() + desktopSize.height()*(1.0 - percent)/3.0,
                   desktopSize.width()*percent,
                   desktopSize.height()*percent);
}
