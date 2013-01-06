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
#include <QtGui/QMenuBar>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>

#include <guisystem/abstractdocument.h>
#include <guisystem/actionmanager.h>
#include <guisystem/command.h>
#include <guisystem/commandcontainer.h>
#include <guisystem/ihistory.h>
#include <guisystem/history.h>
#include <guisystem/menubarcontainer.h>

#include "constants.h"

using namespace GuiSystem;
using namespace Andromeda;

static QByteArray m_windowGeometry;

void BrowserWindowPrivate::setupActions()
{
    Q_Q(BrowserWindow);

    ActionManager *actionManager = ActionManager::instance();

    actions[BrowserWindow::NewTab] = new QAction(q);
    connect(actions[BrowserWindow::NewTab], SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(actions[BrowserWindow::NewTab]);
    actionManager->registerAction(actions[BrowserWindow::NewTab], Constants::Actions::NewTab);

    // ToolBar
    actions[BrowserWindow::Up] = new QAction(QIcon::fromTheme("go-up", QIcon(":/images/icons/up.png")), tr("Up one level"), q);
    connect(actions[BrowserWindow::Up], SIGNAL(triggered()), q, SLOT(up()));
    q->addAction(actions[BrowserWindow::Up]);
    actionManager->registerAction(actions[BrowserWindow::Up], Constants::Actions::Up);

    actions[BrowserWindow::NextTab] = new QAction(q);
#ifdef Q_OS_MAC
    actions[BrowserWindow::NextTab]->setShortcut(QKeySequence(QLatin1String("Ctrl+Right")));
#else
    actions[BrowserWindow::NextTab]->setShortcut(QKeySequence(QLatin1String("Ctrl+Tab")));
#endif
    connect(actions[BrowserWindow::NextTab], SIGNAL(triggered()), q, SLOT(nextTab()));
    q->addAction(actions[BrowserWindow::NextTab]);

    actions[BrowserWindow::PrevTab] = new QAction(q);
#ifdef Q_OS_MAC
    actions[BrowserWindow::PrevTab]->setShortcut(QKeySequence(QLatin1String("Ctrl+Left")));
#else
    actions[BrowserWindow::PrevTab]->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+Tab")));
#endif
    connect(actions[BrowserWindow::PrevTab], SIGNAL(triggered()), q, SLOT(previousTab()));
    q->addAction(actions[BrowserWindow::PrevTab]);

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
    actions[BrowserWindow::Back] = wa;
    actions[BrowserWindow::Back]->setEnabled(false);
    QObject::connect(actions[BrowserWindow::Back], SIGNAL(triggered()), q, SLOT(back()));
    actionManager->registerAction(actions[BrowserWindow::Back], "Back");

    wa = new QWidgetAction(q);
    wa->setDefaultWidget(forwardButton);
    actions[BrowserWindow::Forward] = wa;
    actions[BrowserWindow::Forward]->setEnabled(false);
    QObject::connect(actions[BrowserWindow::Forward], SIGNAL(triggered()), q, SLOT(forward()));
    actionManager->registerAction(actions[BrowserWindow::Forward], "Forward");

    QObject::connect(history, SIGNAL(canGoBackChanged(bool)),
                     actions[BrowserWindow::Back], SLOT(setEnabled(bool)));
    QObject::connect(history, SIGNAL(canGoForwardChanged(bool)),
                     actions[BrowserWindow::Forward], SLOT(setEnabled(bool)));
}

void BrowserWindowPrivate::setupToolBar()
{
    Q_Q(BrowserWindow);

    toolBar = new QToolBar(q);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setObjectName("toolBar");

    toolBar->addAction(q->action(BrowserWindow::Back));
    toolBar->addAction(q->action(BrowserWindow::Forward));
    toolBar->addAction(actions[BrowserWindow::Up]);

    toolBar->addSeparator();
    toolBar->addWidget(lineEdit);

    actions[BrowserWindow::MenuBar] = toolBar->addWidget(q->menuBarButton());
    connect(q, SIGNAL(menuVisibleChanged(bool)), this, SLOT(onMenuVisibleChanged(bool)));

    q->addToolBar(toolBar);
    // TODO: fix Qt bugs
    q->setUnifiedTitleAndToolBarOnMac(true);

    for (int i = 0; i < BrowserWindow::ActionCount; i++) {
        q->addAction(actions[i]);
    }
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
    layout->addWidget(tabWidget);
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
    connect(lineEdit, SIGNAL(refresh()), q, SLOT(reload()));
    connect(lineEdit, SIGNAL(canceled()), q, SLOT(stop()));

    tabWidget = new BrowserTabWidget(/*q*/);
    connect(tabWidget, SIGNAL(editorChanged()), q, SLOT(onEditorChanged()));
    connect(tabWidget, SIGNAL(currentChanged(int)), q, SLOT(onTabChanged()));
//    q->setEditor(container);
//    q->onEditorChanged();
    q->setCentralWidget(tabWidget);

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

void BrowserWindowPrivate::retranslateUi()
{
    backButton->setText(EditorWindow::tr("Back"));
    forwardButton->setText(EditorWindow::tr("Forward"));
}

void BrowserWindowPrivate::onMenuVisibleChanged(bool visible)
{
    actions[BrowserWindow::MenuBar]->setVisible(!visible);
}

BrowserWindowFactory::BrowserWindowFactory(QObject *parent) :
    EditorWindowFactory(parent)
{
}

EditorWindow *BrowserWindowFactory::create()
{
    return new BrowserWindow;
}

void BrowserWindowFactory::open(const QList<QUrl> &urls)
{
    BrowserWindow *window = qobject_cast<BrowserWindow*>(m_activeWindow);
    if (window) {
        if (urls.count() == 1)
            window->open(urls.first());
        else
            window->openNewTabs(urls); // TODO: setting to open in windows instead
    } else {
        BrowserWindow *window = qobject_cast<BrowserWindow*>(create());
        window->openNewTabs(urls); // TODO: setting to open in windows instead
        window->show();
    }
}

void BrowserWindowFactory::openNewEditor(const QList<QUrl> &urls)
{
    BrowserWindow *window = qobject_cast<BrowserWindow*>(m_activeWindow);
    if (window) {
        window->openNewTabs(urls);
    } else {
        BrowserWindow *window = qobject_cast<BrowserWindow*>(create());
        window->openNewTabs(urls);
        window->show();
    }
}

void BrowserWindowFactory::openNewWindow(const QList<QUrl> &urls)
{
    BrowserWindow *window = qobject_cast<BrowserWindow*>(create());
    window->openNewTabs(urls); // TODO: setting to open in windows instead
    window->show();
}

void BrowserWindowFactory::openEditor(const QList<QUrl> &urls, const QByteArray &editor)
{
    BrowserWindow *window = qobject_cast<BrowserWindow*>(m_activeWindow);
    if (window)
        window->openEditor(urls, editor);
}

/*!
    \class Andromeda::BrowserWindow

    \brief BrowserWindow is an Andromeda's main window.

    \image html browserwindow.png
*/

/*!
    Creates BrowserWindow with the given \a parent.
*/
BrowserWindow::BrowserWindow(QWidget *parent) :
    EditorWindow(parent),
    d_ptr(new BrowserWindowPrivate(this))
{
    Q_D(BrowserWindow);

    d->history = new History(this);

    d->setupUi();
    d->setupActions();
    d->setupToolBar();
    d->setupAlternateToolBar();
    d->retranslateUi();

#ifndef Q_OS_MAC
    setMenuBar(MenuBarContainer::instance()->menuBar());
    menuBar()->setVisible(menuVisible());
#endif
    setAttribute(Qt::WA_DeleteOnClose);

//    if ( !(m_windowGeometry.isNull() || m_windowGeometry.isEmpty()) ) {
//        restoreGeometry(m_windowGeometry);
//        move(pos() + QPoint(20, 20));
//    }
}

/*!
    Destroys BrowserWindow.
*/
BrowserWindow::~BrowserWindow()
{
    delete d_ptr;
}

QAction * BrowserWindow::action(Action action) const
{
    if (action < 0 || action >= ActionCount)
        return 0;

    Q_D(const BrowserWindow);
    return d->actions[action];
}

void BrowserWindow::setEditor(AbstractEditor *editor)
{
    EditorWindow::setEditor(editor);
}

/*!
    Returns currently active BrowserWindow, or 0 if there's no active window.
*/
BrowserWindow * BrowserWindow::currentWindow()
{
    return qobject_cast<BrowserWindow*>(qApp->activeWindow());
}

/*!
    Returns list of all windows.
*/
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

bool BrowserWindow::restoreState(const QByteArray &state)
{
    Q_D(const BrowserWindow);

    QByteArray buffer(state);
    QDataStream s(&buffer, QIODevice::ReadOnly);

    QByteArray windowState, tabState;

    s >> windowState;
    s >> tabState;
    if (!EditorWindow::restoreState(windowState))
        return false;

    if (!d->tabWidget->restoreState(tabState))
        return false;
    return true;
}

QByteArray BrowserWindow::saveState() const
{
    Q_D(const BrowserWindow);

    QByteArray state;
    QDataStream s(&state, QIODevice::WriteOnly);

    s << EditorWindow::saveState();
    s << d->tabWidget->saveState();

    return state;
}

void BrowserWindow::back()
{
    Q_D(BrowserWindow);

    if (editor())
        d->history->back();
}

void BrowserWindow::forward()
{
    Q_D(BrowserWindow);

    if (editor())
        d->history->forward();
}

/*!
    Goes one level up in the filesystem.
*/
void BrowserWindow::up()
{
    QUrl url = document()->url();
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

/*!
    \reimp
*/
void BrowserWindow::open(const QUrl &url)
{
    Q_D(BrowserWindow);
    d->tabWidget->open(url);
}

/*!
    Opens \a url in a new tab.
*/
void BrowserWindow::openNewTab(const QUrl &url)
{
    Q_D(BrowserWindow);

    d->tabWidget->newTab(url);
}

/*!
    Opens \a urls list in new tabs.
*/
void BrowserWindow::openNewTabs(const QList<QUrl> &urls)
{
    foreach (const QUrl &url, urls) {
        openNewTab(url);
    }
}

void BrowserWindow::openEditor(const QList<QUrl> &urls, const QByteArray &editor)
{
    Q_D(BrowserWindow);

    // TODO: add setting to open urls in new windows instead
    d->tabWidget->openEditor(urls, editor);
}

/*!
    \reimp
*/
void BrowserWindow::close()
{
    Q_D(BrowserWindow);

    if (d->tabWidget) {
        if (d->tabWidget->count() > 1)
            d->tabWidget->close();
        else
            EditorWindow::close();
    }
}

/*!
    Creates new tab and opens default location.
*/
void BrowserWindow::newTab()
{
    Q_D(BrowserWindow);

    d->tabWidget->newTab();
}

/*!
    Creates new window and opens default location.
*/
void BrowserWindow::newWindow()
{
    QUrl url = QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
    EditorWindow::openNewWindow(url);
}

/*!
    Switches tab bar to the next tab.
*/
void BrowserWindow::nextTab()
{
    Q_D(BrowserWindow);

    if (d->tabWidget) {
        int index = d->tabWidget->currentIndex();
        d->tabWidget->setCurrentIndex(index == d->tabWidget->count() - 1 ? 0 : index + 1);
    }
}

/*!
    Switches tab bar to the previous tab.
*/
void BrowserWindow::previousTab()
{
    Q_D(BrowserWindow);

    if (d->tabWidget) {
        int index = d->tabWidget->currentIndex();
        d->tabWidget->setCurrentIndex(index ? index - 1 : d->tabWidget->count() - 1);
    }
}

/*!
    \reimp
*/
void BrowserWindow::onUrlChanged(const QUrl &url)
{
    Q_D(BrowserWindow);

    d->actions[BrowserWindow::Up]->setEnabled(!(url.path().isEmpty() || url.path() == "/"));
    d->lineEdit->setUrl(url);
    EditorWindow::onUrlChanged(url);
}

/*!
    \reimp
*/
void BrowserWindow::onProgressChanged(int progress)
{
    Q_D(BrowserWindow);
    d->lineEdit->setLoadProgress(progress);
    EditorWindow::onProgressChanged(progress);
}

/*!
    \reimp
*/
void BrowserWindow::onStateChanged(AbstractDocument::State state)
{
    Q_D(BrowserWindow);
    if (state == AbstractDocument::OpenningState) {
        d->lineEdit->startLoad();
    } else if (state == AbstractDocument::OpenningState) {
        d->lineEdit->finishLoad();
    }
    EditorWindow::onStateChanged(state);
}

void BrowserWindow::moveEvent(QMoveEvent*)
{
    m_windowGeometry = saveGeometry();
}

void BrowserWindow::resizeEvent(QResizeEvent*)
{
    m_windowGeometry = saveGeometry();
}

void BrowserWindow::onEditorChanged()
{
    Q_D(BrowserWindow);
    setEditor(d->tabWidget->currentEditor());
}

void BrowserWindow::onTabChanged()
{
    Q_D(BrowserWindow);

    EditorView * view = d->tabWidget->currentView();
    d->history->setHistory(view->history());
}

QByteArray BrowserWindow::windowGeometry()
{
    return m_windowGeometry;
}

void BrowserWindow::setWindowGeometry(const QByteArray &geometry)
{
    m_windowGeometry = geometry;
}
