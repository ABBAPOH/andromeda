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
#include <guisystem/ihistory.h>

#include "constants.h"

using namespace Core;
using namespace GuiSystem;

static QByteArray m_windowGeometry;

void BrowserWindowPrivate::setupActions()
{
    Q_Q(BrowserWindow);

    ActionManager *actionManager = ActionManager::instance();

    newTabAction = new QAction(q);
    connect(newTabAction, SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(newTabAction);
    actionManager->registerAction(newTabAction, Constants::Actions::NewTab);

    // ToolBar
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
    toolBar->setObjectName("toolBar");

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

BrowserWindowFactory::BrowserWindowFactory(QObject *parent) :
    MainWindowFactory(parent)
{
}

MainWindow *BrowserWindowFactory::create()
{
    qDebug("BrowserWindowFactory::create");
    return new BrowserWindow;
}

void BrowserWindowFactory::open(MainWindowFactory::OpenFlag cap, QList<QUrl> urls)
{
    if (urls.isEmpty())
        return;

    switch (cap) {
    case OpenNewEditor : {
        BrowserWindow *window = qobject_cast<BrowserWindow*>(m_activeWindow);
        if (window)
            window->openNewTabs(urls);
        else
            open(OpenNewWindow, urls);
        break;
    }
    case OpenNewWindow : {
        BrowserWindow *window = qobject_cast<BrowserWindow*>(create());
        window->openNewTabs(urls);
        window->show();
        break;
    }
    default:
        MainWindowFactory::open(cap, urls);
        break;
    }
}

BrowserWindow::BrowserWindow(QWidget *parent) :
    MainWindow(parent),
    d_ptr(new BrowserWindowPrivate(this))
{
    Q_D(BrowserWindow);

    d->setupUi();
    d->setupActions();
    d->setupToolBar();
    d->setupAlternateToolBar();

#ifndef Q_OS_MAC
    setMenuBar(ActionManager::instance()->container(Constants::Menus::MenuBar)->menuBar());
#endif
    setAttribute(Qt::WA_DeleteOnClose);

    if ( !(m_windowGeometry.isNull() || m_windowGeometry.isEmpty()) ) {
        restoreGeometry(m_windowGeometry);
        move(pos() + QPoint(20, 20));
    }
}

BrowserWindow::~BrowserWindow()
{
    delete d_ptr;
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

void BrowserWindow::openNewTab(const QUrl &url)
{
    Q_D(BrowserWindow);

    d->container->openNewEditor(url);
}

void BrowserWindow::openNewTabs(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        openNewTab(url);
    }
}

void BrowserWindow::newTab()
{
    Q_D(BrowserWindow);

    d->container->newTab();
}

void BrowserWindow::newWindow()
{
    QUrl url = QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    MainWindow::openNewWindow(url);
}

void BrowserWindow::moveEvent(QMoveEvent*)
{
    m_windowGeometry = saveGeometry();
}

void BrowserWindow::resizeEvent(QResizeEvent*)
{
    m_windowGeometry = saveGeometry();
}

QByteArray BrowserWindow::windowGeometry()
{
    return m_windowGeometry;
}

void BrowserWindow::setWindowGeometry(const QByteArray &geometry)
{
    m_windowGeometry = geometry;
}
