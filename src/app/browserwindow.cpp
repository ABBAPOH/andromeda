#include "browserwindow.h"
#include "browserwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidgetAction>
#else
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QMenuBar>
#include <QtGui/QToolButton>
#include <QtGui/QWidgetAction>
#endif

#include <Parts/AbstractDocument>
#include <Parts/ActionManager>
#include <Parts/Command>
#include <Parts/CommandContainer>
#include <Parts/IHistory>
#include <Parts/History>
#include <Parts/ToolBar>
#include <Parts/constants.h>

using namespace Parts;
using namespace Andromeda;

static QByteArray m_windowGeometry;

void BrowserWindowPrivate::setupActions()
{
    Q_Q(BrowserWindow);

    actions[BrowserWindow::NewTab] = new QAction(q);
    actions[BrowserWindow::NewTab]->setObjectName(Constants::Actions::NewTab);
    connect(actions[BrowserWindow::NewTab], SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(actions[BrowserWindow::NewTab]);

    actions[BrowserWindow::Up] = new QAction(q);
    actions[BrowserWindow::Up]->setText(tr("Up one level"));
    actions[BrowserWindow::Up]->setIcon(QIcon::fromTheme("go-up", QIcon(":/andromeda/icons/up.png")));
    actions[BrowserWindow::Up]->setObjectName(Constants::Actions::Up);
    connect(actions[BrowserWindow::Up], SIGNAL(triggered()), q, SLOT(up()));
    q->addAction(actions[BrowserWindow::Up]);

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
}

void BrowserWindowPrivate::setupToolBar()
{
    Q_Q(BrowserWindow);

    ToolBar *toolBar = new ToolBar(q);
    this->toolBar = toolBar;
    toolBar->setContainer("ToolBar");
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setObjectName("toolBar");
    toolBar->setContextMenuPolicy(Qt::CustomContextMenu);

#ifndef Q_OS_MAC
    actions[BrowserWindow::MenuBar] = toolBar->addWidget(q->menuBarButton());
#else
    actions[BrowserWindow::MenuBar] = 0;
#endif
    connect(q, SIGNAL(menuVisibleChanged(bool)), this, SLOT(onMenuVisibleChanged(bool)));

    q->addToolBar(Qt::TopToolBarArea, toolBar);
    // TODO: fix Qt bugs
//    q->setUnifiedTitleAndToolBarOnMac(true);

    for (int i = 0; i < BrowserWindow::ActionCount; i++) {
        if (actions[i])
            q->addAction(actions[i]);
    }
}

void BrowserWindowPrivate::setupAlternateToolBar()
{
    Q_Q(BrowserWindow);

    CommandContainer *c = ActionManager::instance()->container(Constants::Objects::AlternateToolbar);
    if (!c)
        return;

    ToolBar *toolBar = new ToolBar(q);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);
    toolBar->setObjectName("AlternateToolBar");
    toolBar->setContainer(c);
    toolBar->setIconSize(QSize(16, 16));

//    if (!toolBar)
//        return;

#if 0
//#if defined(Q_WS_MAC)
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
    q->addToolBarBreak();
    q->addToolBar(Qt::TopToolBarArea, toolBar);
#endif
}

void BrowserWindowPrivate::setupUi()
{
    Q_Q(BrowserWindow);

    tabWidget = new BrowserTabWidget(/*q*/);
    connect(tabWidget, SIGNAL(editorChanged()), q, SLOT(onEditorChanged()));
    connect(tabWidget, SIGNAL(currentChanged(int)), q, SLOT(onTabChanged()));
//    q->setEditor(container);
//    q->onEditorChanged();
    q->setCentralWidget(tabWidget);

    q->resize(800, 600);
}

void BrowserWindowPrivate::retranslateUi()
{
//    backButton->setText(EditorWindow::tr("Back"));
//    forwardButton->setText(EditorWindow::tr("Forward"));
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

History * BrowserWindow::history() const
{
    Q_D(const BrowserWindow);
    return d->history;
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
    QUrl url = QUrl::fromLocalFile(QDir::homePath());
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
    EditorWindow::onUrlChanged(url);
    emit urlChanged(url);
}

/*!
    \reimp
*/
void BrowserWindow::onProgressChanged(int progress)
{
    Q_D(BrowserWindow);

    AddressBar *bar = d->toolBar->findChild<AddressBar *>();
    if (!bar)
        return;
    bar->setLoadProgress(progress);
    EditorWindow::onProgressChanged(progress);
}

/*!
    \reimp
*/
void BrowserWindow::onStateChanged(AbstractDocument::State state)
{
    Q_D(BrowserWindow);
    AddressBar *bar = d->toolBar->findChild<AddressBar *>();
    if (!bar)
        return;

    if (state == AbstractDocument::OpenningState) {
        bar->startLoad();
    } else if (state == AbstractDocument::OpenningState) {
        bar->finishLoad();
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
