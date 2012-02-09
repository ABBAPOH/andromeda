#include "mainwindow.h"
#include "mainwindow_p.h"

#include "abstractcontainer.h"
#include "actionmanager.h"
#include "abstracthistory.h"
#include "commandcontainer.h"
#include "stackedcontainer.h"

#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QFileDialog>

static const qint32 mainWindowMagic = 0x6d303877; // "m08w"
static const qint8 mainWindowVersion = 1;

using namespace GuiSystem;

MainWindow::CreateWindowFunc MainWindow::createWindowFunc = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->contanier = 0;
    d->createActions();
    d->retranslateUi();
    d->registerActions();

    setMenuBar(ActionManager::instance()->container("MenuBar")->menuBar());

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

AbstractContainer * MainWindow::contanier() const
{
    Q_D(const MainWindow);

    return d->contanier;
}

void MainWindow::setContanier(AbstractContainer *container)
{
    Q_D(MainWindow);

    if (d->contanier == container)
        return;

    if (d->contanier) {
        disconnect(d->contanier, 0, this, 0);

        disconnect(d->contanier->history(), 0, d->actions[Back], 0);
        disconnect(d->contanier->history(), 0, d->actions[Forward], 0);
        disconnect(d->contanier, 0, d->actions[Save], 0);
    }

    d->contanier = container;

    connect(d->contanier, SIGNAL(openTriggered(QUrl)), SLOT(open(QUrl)));
    connect(d->contanier, SIGNAL(openNewEditorTriggered(QList<QUrl>)), SLOT(openNewEditor(QList<QUrl>)));
    connect(d->contanier, SIGNAL(openNewWindowTriggered(QList<QUrl>)), SLOT(openNewWindow(QList<QUrl>)));

    connect(d->contanier, SIGNAL(iconChanged(QIcon)), SLOT(setWindowIcon(QIcon)));
    connect(d->contanier, SIGNAL(windowTitleChanged(QString)), SLOT(setWindowTitle(QString)));

    connect(d->contanier->history(), SIGNAL(canGoBackChanged(bool)),
            d->actions[Back], SLOT(setEnabled(bool)));
    connect(d->contanier->history(), SIGNAL(canGoForwardChanged(bool)),
            d->actions[Forward], SLOT(setEnabled(bool)));

    connect(d->contanier, SIGNAL(modificationChanged(bool)), d->actions[Save], SLOT(setEnabled(bool)));

    d->actions[Back]->setEnabled(d->contanier->history()->canGoBack());
    d->actions[Forward]->setEnabled(d->contanier->history()->canGoForward());

    bool saveAsEnabled = d->contanier->capabilities() & AbstractEditor::CanSave;
    bool saveEnabled = saveAsEnabled && d->contanier->isModified() && !d->contanier->isReadOnly();
    d->actions[SaveAs]->setEnabled(saveAsEnabled);
    d->actions[Save]->setEnabled(saveEnabled);

    setCentralWidget(d->contanier);
}

QUrl MainWindow::url() const
{
    Q_D(const MainWindow);

    if (d->contanier)
        return d->contanier->url();

    return QUrl();
}

QList<QUrl> MainWindow::urls() const
{
    Q_D(const MainWindow);

    if (d->contanier)
        return d->contanier->urls();

    return QList<QUrl>();
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
    if (createWindowFunc)
        return createWindowFunc();
    else
        qWarning() << "MainWindow::createWindow:" << "createWindowFunc is not set";

    return 0;
}

void MainWindow::restoreState(const QByteArray &arr)
{
    Q_D(MainWindow);

    QByteArray state = arr;

    QBuffer buffer(&state);
    buffer.open(QBuffer::ReadOnly);
    QDataStream s(&buffer);

    qint32 magic;
    qint8 version;
    QByteArray geometry;
    QByteArray mainWindowState;
    QByteArray containerState;

    s >> magic;
    if (magic != mainWindowMagic)
        return;

    s >> version;
    if (version != mainWindowVersion)
        return;

    s >> geometry;
    s >> mainWindowState;
    s >> containerState;
    restoreGeometry(geometry);
    QMainWindow::restoreState(mainWindowState);

    if (d->contanier)
        d->contanier->restoreState(containerState);
}

QByteArray MainWindow::saveState() const
{
    Q_D(const MainWindow);

    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);
    QDataStream s(&buffer);

    s << mainWindowMagic;
    s << mainWindowVersion;
    s << saveGeometry();
    s << QMainWindow::saveState();
    if (d->contanier)
        s << d->contanier->saveState();

    return buffer.data();
}

void MainWindow::back()
{
    Q_D(MainWindow);

    if (d->contanier)
        if (d->contanier->capabilities() & AbstractEditor::HasHistory)
            d->contanier->history()->back();
}

void MainWindow::forward()
{
    Q_D(MainWindow);

    if (d->contanier)
        if (d->contanier->capabilities() & AbstractEditor::HasHistory)
            d->contanier->history()->forward();
}

void MainWindow::open(const QUrl &url)
{
    Q_D(const MainWindow);

    if (d->contanier)
        d->contanier->open(url);
}

void MainWindow::openEditor(const QString &id)
{
    QUrl url;
    url.setScheme(qApp->applicationName());
    url.setHost(id);
    open(url);
}

void MainWindow::openNewEditor(const QUrl &url)
{
    Q_D(MainWindow);

    if (d->contanier)
        d->contanier->openNewEditor(url);
}

void MainWindow::openNewEditor(const QList<QUrl> &urls)
{
    Q_D(MainWindow);

    if (d->contanier) {
        d->contanier->openNewEditor(urls);
    }
}

void MainWindow::closeEditor()
{
    Q_D(MainWindow);

    if (d->contanier) {
        if (d->contanier->count() > 1)
            d->contanier->closeEditor(d->contanier->currentIndex());
        else
            close();
    }
}

void MainWindow::openNewWindow(const QUrl &url)
{
    return openNewWindow(QList<QUrl>() << url);
}

void MainWindow::openNewWindow(const QList<QUrl> &urls)
{
    MainWindow *window = new MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->setContanier(new StackedContainer(window));
    foreach (const QUrl &url, urls) {
        window->openNewEditor(url);
    }
    window->show();
}

void MainWindow::save()
{
    Q_D(MainWindow);

    if (d->contanier)
        return;

    if (!(d->contanier->capabilities() & AbstractEditor::CanSave))
        return;

    d->contanier->save();
}

void MainWindow::saveAs()
{
    Q_D(MainWindow);

    if (!d->contanier)
        return;

    // TODO: remember previous dir and set filename using title + extension from mimetype
    QString path = QFileDialog::getSaveFileName(this, tr("Save as"));

    if (path.isEmpty())
        return;

    if (!(d->contanier->capabilities() & AbstractEditor::CanSave))
        return;

    d->contanier->save(QUrl::fromLocalFile(path));
}

void MainWindow::refresh()
{
    Q_D(MainWindow);

    if (d->contanier)
        d->contanier->refresh();
}

void MainWindow::cancel()
{
    Q_D(MainWindow);

    if (d->contanier)
        d->contanier->cancel();
}

void MainWindow::nextEditor()
{
    Q_D(MainWindow);

    if (d->contanier) {
        int index = d->contanier->currentIndex();
        d->contanier->setCurrentIndex(index == d->contanier->count() - 1 ? 0 : index + 1);
    }
}

void MainWindow::previousEditor()
{
    Q_D(MainWindow);

    if (d->contanier) {
        int index = d->contanier->currentIndex();
        d->contanier->setCurrentIndex(index ? index - 1 : d->contanier->count() - 1);
    }
}

void MainWindow::setWindowIcon(const QIcon &icon)
{
    QMainWindow::setWindowIcon(icon);
}

void MainWindowPrivate::createActions()
{
    Q_Q(MainWindow);

    actions[MainWindow::OpenFile] = new QAction(q);

    actions[MainWindow::Close] = new QAction(q);
    QObject::connect(actions[MainWindow::Close], SIGNAL(triggered()), q, SLOT(closeEditor()));

    actions[MainWindow::Save] = new QAction(q);
    QObject::connect(actions[MainWindow::Save], SIGNAL(triggered()), q, SLOT(save()));

    actions[MainWindow::SaveAs] = new QAction(q);
    QObject::connect(actions[MainWindow::SaveAs], SIGNAL(triggered()), q, SLOT(saveAs()));

    actions[MainWindow::Refresh] = new QAction(q);
    QObject::connect(actions[MainWindow::Refresh], SIGNAL(triggered()), q, SLOT(refresh()));

    actions[MainWindow::Cancel] = new QAction(q);
    QObject::connect(actions[MainWindow::Cancel], SIGNAL(triggered()), q, SLOT(cancel()));

    actions[MainWindow::Back] = new QAction(q);
    actions[MainWindow::Back]->setShortcut(QKeySequence::Back);
    QObject::connect(actions[MainWindow::Back], SIGNAL(triggered()), q, SLOT(back()));

    actions[MainWindow::Forward] = new QAction(q);
    actions[MainWindow::Forward]->setShortcut(QKeySequence::Forward);
    QObject::connect(actions[MainWindow::Forward], SIGNAL(triggered()), q, SLOT(forward()));

    actions[MainWindow::NextEditor] = new QAction(q);
#ifdef Q_OS_MAC
    actions[MainWindow::NextEditor]->setShortcut(QKeySequence(QLatin1String("Ctrl+Right")));
#else
    actions[MainWindow::NextEditor]->setShortcut(QKeySequence(QLatin1String("Ctrl+Tab")));
#endif
    QObject::connect(actions[MainWindow::NextEditor], SIGNAL(triggered()), q, SLOT(nextEditor()));

    actions[MainWindow::PreviousEditor] = new QAction(q);
#ifdef Q_OS_MAC
    actions[MainWindow::PreviousEditor]->setShortcut(QKeySequence(QLatin1String("Ctrl+Left")));
#else
    actions[MainWindow::PreviousEditor]->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+Tab")));
#endif
    QObject::connect(actions[MainWindow::PreviousEditor], SIGNAL(triggered()), q, SLOT(previousEditor()));

    for (int i = 0; i < MainWindow::ActionCount; i++) {
        q->addAction(actions[i]);
    }
}

void MainWindowPrivate::retranslateUi()
{
    actions[MainWindow::Save]->setText(QObject::tr("Save..."));
    actions[MainWindow::SaveAs]->setText(QObject::tr("Save as..."));
    actions[MainWindow::Refresh]->setText(QObject::tr("Refresh"));
    actions[MainWindow::Cancel]->setText(QObject::tr("Cancel"));
    actions[MainWindow::Back]->setText(QObject::tr("Back"));
    actions[MainWindow::Forward]->setText(QObject::tr("Forward"));

    actions[MainWindow::NextEditor]->setText(QObject::tr("Next editor"));
    actions[MainWindow::PreviousEditor]->setText(QObject::tr("Previous editor"));
}

void MainWindowPrivate::registerActions()
{
    ActionManager *manager = ActionManager::instance();
    manager->registerAction(actions[MainWindow::Close], "Actions:6.CloseTab");

    manager->registerAction(actions[MainWindow::Back], "Actions.Back");
    manager->registerAction(actions[MainWindow::Forward], "Actions.Forward");
}

void MainWindowPrivate::initGeometry()
{
    Q_Q(MainWindow);

    static const float percent = 0.58;
    QDesktopWidget desktop;
    QRect desktopRect = desktop.availableGeometry(q);
    QSize desktopSize = QSize(desktopRect.width(), desktopRect.height());
    q->setGeometry(desktopRect.x() + desktopSize.width()*(1.0 - percent)/2.0,
                   desktopRect.y() + desktopSize.height()*(1.0 - percent)/3.0,
                   desktopSize.width()*percent,
                   desktopSize.height()*percent);
}
