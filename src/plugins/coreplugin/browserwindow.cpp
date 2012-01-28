#include "browserwindow.h"
#include "browserwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QCompleter>
#include <QtGui/QDesktopServices>
#include <QtGui/QDirModel>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>

#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/commandcontainer.h>
#include <guisystem/abstracthistory.h>
//#include <coreplugin/CorePlugin>
#include <coreplugin/constants.h>

using namespace CorePlugin;
using namespace GuiSystem;

void BrowserWindowPrivate::setupActions()
{
    Q_Q(BrowserWindow);

    ActionManager *actionManager = ActionManager::instance();

    newTabAction = new QAction(q);
    connect(newTabAction, SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(newTabAction);
    actionManager->registerAction(newTabAction, Constants::Actions::NewTab);

    // ToolBar
    q->action(MainWindow::Back)->setIcon(QIcon::fromTheme("go-previous", QIcon(":/images/icons/back.png")));
    q->action(MainWindow::Forward)->setIcon(QIcon::fromTheme("go-next", QIcon(":/images/icons/forward.png")));

    upAction = new QAction(QIcon::fromTheme("go-up", QIcon(":/images/icons/up.png")), tr("Up one level"), q);
    connect(upAction, SIGNAL(triggered()), q, SLOT(up()));
    q->addAction(upAction);
    actionManager->registerAction(upAction, Constants::Actions::Up);
}

void BrowserWindowPrivate::setupToolBar()
{
    Q_Q(BrowserWindow);

    toolBar = new QToolBar(q);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    toolBar->addAction(q->action(MainWindow::Back));
    toolBar->addAction(q->action(MainWindow::Forward));
    toolBar->addAction(upAction);

    toolBar->addSeparator();
    toolBar->addWidget(lineEdit);

    q->addToolBar(toolBar);
    q->setUnifiedTitleAndToolBarOnMac(true);
}

void BrowserWindowPrivate::setupAlternateToolBar()
{
    Q_Q(BrowserWindow);

    CommandContainer *c = ActionManager::instance()->container(Constants::Objects::AlternateToolbar);
    if (!c)
        return;

    QToolBar *toolBar = c->toolBar(q);

    if (!toolBar)
        return;

#if defined(Q_WS_MAC)
    QWidget *centralWidget = new QWidget(q);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QFrame *frame = new QFrame(q);
    QPalette framePalette = frame->palette();
    framePalette.setColor(QPalette::Active, QPalette::Light, QColor(64, 64, 64));
    framePalette.setColor(QPalette::Active, QPalette::Dark, QColor(192, 192, 192));
    framePalette.setColor(QPalette::Inactive, QPalette::Light, QColor(135, 135, 135));
    framePalette.setColor(QPalette::Inactive, QPalette::Dark, QColor(226, 226, 226));

    frame->setLineWidth(1);
    frame->setMidLineWidth(0);
    frame->setFrameShape(QFrame::HLine);
    frame->setFrameShadow(QFrame::Raised);
    frame->setAttribute(Qt::WA_MacNoClickThrough, true);
    frame->setPalette(framePalette);

    QPalette toolBarPalette = toolBar->palette();
    toolBarPalette.setColor(QPalette::Active, QPalette::Window, QColor(167, 167, 167));
    toolBarPalette.setColor(QPalette::Inactive, QPalette::Window, QColor(207, 207, 207));

    toolBar->setAutoFillBackground(true);
    toolBar->setFixedHeight(19);
    toolBar->setAttribute(Qt::WA_MacNoClickThrough, true);
    toolBar->setBackgroundRole(QPalette::Window);
    toolBar->setPalette(toolBarPalette);

    layout->addWidget(frame);
    layout->addWidget(toolBar);
    layout->addWidget(container);
//    layout->addWidget(tabWidget);
    q->setCentralWidget(centralWidget);
#else
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
    q->addToolBarBreak();
    q->addToolBar(toolBar);
#endif
}

void BrowserWindowPrivate::setupUi()
{
    Q_Q(BrowserWindow);

    lineEdit = new MyAddressBar(q);
    lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
//    lineEdit->setStyleSheet("QLineEdit { border-radius: 2px; }");
    connect(lineEdit, SIGNAL(open(QUrl)), q, SLOT(open(QUrl)));
    connect(lineEdit, SIGNAL(refresh()), q, SLOT(refresh()));
    connect(lineEdit, SIGNAL(canceled()), q, SLOT(cancel()));

    container = new TabContainer(/*q*/);
    q->setContanier(container);

    connect(container, SIGNAL(urlChanged(QUrl)), this, SLOT(onUrlChanged(QUrl)));
    connect(container, SIGNAL(urlChanged(QUrl)), lineEdit, SLOT(setUrl(QUrl)));
    connect(container, SIGNAL(loadStarted()), lineEdit, SLOT(startLoad()));
    connect(container, SIGNAL(loadProgress(int)), lineEdit, SLOT(setLoadProgress(int)));
    connect(container, SIGNAL(loadFinished(bool)), lineEdit, SLOT(finishLoad()));

// ### fixme QDirModel is used in QCompleter because QFileSystemModel seems broken
// This is an example how to use completers to help directory listing.
// I'm not sure if it shouldn't be a part of plugins (standalone for web...)
// TODO/FIXME: QFileSystemModel is probably broken for qcompleter so the obsolete
//             QDirModel is used here.
//    QFileSystemModel * dirModel = new QFileSystemModel(this);
    QDirModel *dirModel = new QDirModel(this);
//    dirModel->setRootPath(QDir::rootPath());
    QCompleter *completer = new QCompleter(dirModel, lineEdit);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    lineEdit->setCompleter(completer);

    q->resize(800, 600);
}

void BrowserWindowPrivate::onUrlChanged(const QUrl &url)
{
    upAction->setEnabled(!(url.path().isEmpty() || url.path() == "/"));
}

BrowserWindow::BrowserWindow(QWidget *parent) :
    MainWindow(parent),
    d_ptr(new BrowserWindowPrivate(this))
{
    createWindowFunc = (CreateWindowFunc)createWindow;

    Q_D(BrowserWindow);

    d->setupUi();
    d->setupActions();
    d->setupToolBar();
    d->setupAlternateToolBar();

    setMenuBar(ActionManager::instance()->container(Constants::Menus::MenuBar)->menuBar());
    setAttribute(Qt::WA_DeleteOnClose);

    d->settings = new QSettings(this);
    d->settings->beginGroup(QLatin1String("mainWindow"));
    QVariant value = d->settings->value(QLatin1String("geometry"));
    if (value.isValid()) {
        restoreGeometry(value.toByteArray());
        move(pos() + QPoint(20, 20));
    }
}

BrowserWindow::~BrowserWindow()
{
    delete d_ptr;
}

void BrowserWindow::restoreSession(QSettings &s)
{
    MainWindow::restoreState(s.value(QLatin1String("state")).toByteArray());
}

void BrowserWindow::saveSession(QSettings &s)
{
    s.setValue(QLatin1String("state"), MainWindow::saveState());
}

BrowserWindow * BrowserWindow::currentWindow()
{
    return qobject_cast<BrowserWindow*>(qApp->activeWindow());
}

QList<BrowserWindow *> BrowserWindow::windows()
{
    QList<BrowserWindow*> result;
    foreach (QWidget *widget, qApp->topLevelWidgets()) {
        BrowserWindow* window = qobject_cast<BrowserWindow*>(widget);
        if (window)
            result.append(window);
    }
    return result;
}

BrowserWindow * BrowserWindow::createWindow()
{
    return new BrowserWindow();
}

void BrowserWindow::up()
{
    Q_D(BrowserWindow);

    QUrl url = d->container->url();
    QString path = url.path();
    // we can't use QDir::cleanPath because it breaks urls
    // remove / at end of path
    if (path != QLatin1String("/"))
        if (path.endsWith(QLatin1Char('/')))
            path = path.left(path.length() - 1);

    QFileInfo info(path);
    url.setPath(info.path());
    open(url);
}

void BrowserWindow::openNewWindow(const QUrl &url)
{
    MainWindow::openNewWindow(url);
}

void BrowserWindow::openNewWindow(const QList<QUrl> &urls)
{
    BrowserWindow *window = createWindow();
    foreach (const QUrl & url, urls) {
        window->openNewEditor(url);
    }
    window->show();
}

void BrowserWindow::newTab()
{
    Q_D(BrowserWindow);

    d->container->newTab();
}

void BrowserWindow::newWindow()
{
    QUrl url = QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    BrowserWindow *window = createWindow();
    window->openNewEditor(url);
    window->show();
}

void BrowserWindow::moveEvent(QMoveEvent *)
{
    Q_D(BrowserWindow);

    d->settings->setValue(QLatin1String("geometry"), saveGeometry());
}

void BrowserWindow::resizeEvent(QResizeEvent *)
{
    Q_D(BrowserWindow);

    d->settings->setValue(QLatin1String("geometry"), saveGeometry());
}
