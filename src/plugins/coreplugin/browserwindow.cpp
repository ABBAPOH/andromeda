#include "browserwindow.h"
#include "browserwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QSignalMapper>

#include <QtGui/QAction>
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

StackedEditor * BrowserWindowPrivate::addTab(int *index)
{
    Q_Q(BrowserWindow);

    StackedEditor *tab = new StackedEditor(tabWidget);
    connect(tab, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
    connect(tab, SIGNAL(openNewEditorTriggered(QList<QUrl>)), q, SLOT(openNewTab(QList<QUrl>)));
    connect(tab, SIGNAL(openNewWindowTriggered(QList<QUrl>)), q, SLOT(openNewWindow(QList<QUrl>)));
    connect(tab, SIGNAL(iconChanged(QIcon)), SLOT(onChanged()));
    connect(tab, SIGNAL(titleChanged(QString)), SLOT(onChanged()));
    connect(tab, SIGNAL(windowTitleChanged(QString)), SLOT(onChanged()));
    int i = tabWidget->addTab(tab, "tab");
    if (index)
        *index = i;

    return tab;
}

void BrowserWindowPrivate::createAction(QAction *&action, const QString &text, const QByteArray &id, QWidget *w, const char *slot)
{
    ActionManager *actionManager = ActionManager::instance();

    action = new QAction(this);
    action->setText(text);
    connect(action, SIGNAL(triggered()), w, slot);
    w->addAction(action);
    actionManager->registerAction(action, id);
}

void BrowserWindowPrivate::setupActions()
{
    Q_Q(BrowserWindow);

    ActionManager *actionManager = ActionManager::instance();

    newTabAction = new QAction(this);
    connect(newTabAction, SIGNAL(triggered()), q, SLOT(newTab()));
    q->addAction(newTabAction);
    actionManager->registerAction(newTabAction, Constants::Actions::NewTab);

    closeTabAction = new QAction(this);
    connect(closeTabAction, SIGNAL(triggered()), q, SLOT(closeTab()));
    q->addAction(closeTabAction);
    actionManager->registerAction(closeTabAction, Constants::Actions::CloseTab);

    saveAction = new QAction(this);
    saveAction->setEnabled(false);
    connect(saveAction, SIGNAL(triggered()), q, SLOT(save()));
    q->addAction(saveAction);
    actionManager->registerAction(saveAction, Constants::Actions::Save);

    saveAsAction = new QAction(this);
    saveAsAction->setEnabled(false);
    connect(saveAsAction, SIGNAL(triggered()), q, SLOT(saveAs()));
    q->addAction(saveAsAction);
    actionManager->registerAction(saveAsAction, Constants::Actions::SaveAs);

    // LineEdit
    createAction(undoAction, tr("Undo"), Constants::Actions::Undo, lineEdit, SLOT(undo()));
    createAction(redoAction, tr("Redo"), Constants::Actions::Redo, lineEdit, SLOT(redo()));

    QAction *a = new QAction(this);
    a->setSeparator(true);
    lineEdit->addAction(a);

    createAction(cutAction, tr("Cut"), Constants::Actions::Cut, lineEdit, SLOT(cut()));
    createAction(copyAction, tr("Copy"), Constants::Actions::Copy, lineEdit, SLOT(copy()));
    createAction(pasteAction, tr("Paste"), Constants::Actions::Paste, lineEdit, SLOT(paste()));
    createAction(selectAllAction, tr("Select All"), Constants::Actions::SelectAll, lineEdit, SLOT(selectAll()));

    // ToolBar
    backAction = new QAction(QIcon::fromTheme("go-previous", QIcon(":/images/icons/back.png")), tr("Back"), this);
    connect(backAction, SIGNAL(triggered()), q, SLOT(back()));
    q->addAction(backAction);
    actionManager->registerAction(backAction, Constants::Actions::Back);

    forwardAction = new QAction(QIcon::fromTheme("go-next", QIcon(":/images/icons/forward.png")), tr("Forward"), this);
    connect(forwardAction, SIGNAL(triggered()), q, SLOT(forward()));
    q->addAction(forwardAction);
    actionManager->registerAction(forwardAction, Constants::Actions::Forward);

    upAction = new QAction(QIcon::fromTheme("go-up", QIcon(":/images/icons/up.png")), tr("Up one level"), this);
    connect(upAction, SIGNAL(triggered()), q, SLOT(up()));
    q->addAction(upAction);
    actionManager->registerAction(upAction, Constants::Actions::Up);

    nextTabAction = new QAction(this);
#ifdef Q_OS_MAC
    nextTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Right")));
#else
    nextTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Tab")));
#endif
    nextTabAction->setShortcutContext(Qt::WindowShortcut);
    connect(nextTabAction, SIGNAL(triggered()), q, SLOT(nextTab()));
    q->addAction(nextTabAction);

    prevTabAction = new QAction(q);
#ifdef Q_OS_MAC
    prevTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Left")));
#else
    prevTabAction->setShortcut(QKeySequence(QLatin1String("Ctrl+Shift+Tab")));
#endif
    prevTabAction->setShortcutContext(Qt::WindowShortcut);
    connect(prevTabAction, SIGNAL(triggered()), q, SLOT(prevTab()));
    q->addAction(prevTabAction);
}

void BrowserWindowPrivate::setupToolBar()
{
    Q_Q(BrowserWindow);

    toolBar = new QToolBar(q);
    toolBar->setFloatable(false);
    toolBar->setMovable(false);

    toolBar->addAction(backAction);
    toolBar->addAction(forwardAction);
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
    layout->addWidget(tabWidget);
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

    newTabButton = new TabBarButton();
    newTabButton->setIcon(QIcon(":/images/icons/addtab.png"));
    newTabButton->setIconSize(QSize(32,32));
    connect(newTabButton, SIGNAL(clicked()), q, SLOT(newTab()));

    tabWidget = new MyTabWidget;
    tabWidget->setDocumentMode(true);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    tabWidget->setUsesScrollButtons(true);
    tabWidget->setCornerWidget(newTabButton);

    q->setCentralWidget(tabWidget);
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentChanged(int)));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), q, SLOT(closeTab(int)));
    connect(tabWidget, SIGNAL(tabBarDoubleClicked()), q, SLOT(newTab()));

    lineEdit = new AddressBar(q);
    lineEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
//    lineEdit->setStyleSheet("QLineEdit { border-radius: 2px; }");
    connect(lineEdit, SIGNAL(open(QUrl)), q, SLOT(open(QUrl)));
    connect(lineEdit, SIGNAL(refresh()), q, SLOT(refresh()));
    connect(lineEdit, SIGNAL(canceled()), q, SLOT(cancel()));

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

void BrowserWindowPrivate::updateUi(StackedEditor *tab)
{
    Q_Q(BrowserWindow);

    int index = tabWidget->indexOf(tab);
    tabWidget->setTabText(index, tab->title());
#ifndef Q_OS_MAC
    tabWidget->setTabIcon(index, tab->icon());
#endif

    if (tab == q->currentTab()) {
        QString windowTitle = tab->windowTitle();
        windowTitle = windowTitle.isEmpty() ? tab->title() : windowTitle;
        if (windowTitle.isEmpty())
            q->setWindowTitle(QString(tr("Andromeda")));
        else
            q->setWindowTitle(QString(tr("%1 - Andromeda").arg(windowTitle)));

        QIcon windowIcon = tab->icon();
        windowIcon = windowIcon.isNull() ? QIcon(":/images/icons/andromeda.png") : windowIcon;
        q->setWindowIcon(windowIcon);
    }
}

void BrowserWindowPrivate::onUrlChanged(const QUrl &url)
{
    Q_Q(BrowserWindow);

    if (sender() == q->currentTab())
        lineEdit->setUrl(url);

    upAction->setEnabled(!(url.path().isEmpty() || url.path() == "/"));
}

void BrowserWindowPrivate::onCurrentChanged(int index)
{
    StackedEditor *tab = qobject_cast<StackedEditor *>(tabWidget->widget(index));
    if (!tab)
        return;

    lineEdit->setLoading(false);
    disconnect(tab, 0, lineEdit, 0);
    connect(tab, SIGNAL(loadStarted()), lineEdit,  SLOT(startLoad()));
    connect(tab, SIGNAL(loadProgress(int)), lineEdit,  SLOT(setLoadProgress(int)));
    connect(tab, SIGNAL(loadFinished(bool)), lineEdit,  SLOT(finishLoad()));

    disconnect(tab->history(), 0, backAction, 0);
    disconnect(tab->history(), 0, forwardAction, 0);
    connect(tab->history(), SIGNAL(canGoBackChanged(bool)), backAction, SLOT(setEnabled(bool)));
    connect(tab->history(), SIGNAL(canGoForwardChanged(bool)), forwardAction, SLOT(setEnabled(bool)));

    disconnect(tab, 0, saveAction, 0);
    connect(tab, SIGNAL(modificationChanged(bool)), saveAction, SLOT(setEnabled(bool)));

    lineEdit->setUrl(tab->url());

    updateUi(tab);

    bool saveAsEnabled = tab->capabilities() & AbstractEditor::CanSave;
    bool saveEnabled = saveAsEnabled && tab->isModified() && !tab->isReadOnly();
    saveAsAction->setEnabled(saveAsEnabled);
    saveAction->setEnabled(saveEnabled);
}

void BrowserWindowPrivate::onChanged()
{
    StackedEditor *tab = qobject_cast<StackedEditor *>(sender());
    if (!tab)
        return;

    updateUi(tab);
}

void BrowserWindowPrivate::onCapabilitiesChanged(AbstractEditor::Capabilities capabilities)
{
    bool saveAsEnabled = capabilities & AbstractEditor::CanSave;

    saveAsAction->setEnabled(saveAsEnabled);

    if (!saveAsEnabled)
        saveAction->setEnabled(false);
}

BrowserWindow::BrowserWindow(QWidget *parent) :
    QMainWindow(parent),
    d_ptr(new BrowserWindowPrivate(this))
{
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

int BrowserWindow::currentIndex() const
{
    return d_func()->tabWidget->currentIndex();
}

StackedEditor * BrowserWindow::currentTab() const
{
    return qobject_cast<StackedEditor *>(d_func()->tabWidget->currentWidget());
}

int BrowserWindow::count() const
{
    return d_func()->tabWidget->count();
}

void BrowserWindow::restoreSession(QSettings &s)
{
    Q_D(BrowserWindow);

    setGeometry(s.value(QLatin1String("geometry")).toRect());

    restoreState(s.value(QLatin1String("state")).toByteArray());

    int tabCount = s.beginReadArray(QLatin1String("tabs"));
    for (int i = 0; i < tabCount; i++) {
        s.setArrayIndex(i);

        StackedEditor *tab = d->addTab();
        tab->restoreState(s.value(QLatin1String("tab")).toByteArray());
    }
    s.endArray();

    d->tabWidget->setCurrentIndex(s.value(QLatin1String("currentTab")).toInt());
}

void BrowserWindow::saveSession(QSettings &s)
{
    Q_D(BrowserWindow);

    s.setValue(QLatin1String("geometry"), geometry());
    s.setValue(QLatin1String("state"), saveState());
    s.setValue(QLatin1String("currentTab"), d->tabWidget->currentIndex());

    int tabCount = d->tabWidget->count();
    s.beginWriteArray(QLatin1String("tabs"), tabCount);
    for (int i = 0; i < tabCount; i++) {
        StackedEditor *tab = static_cast<StackedEditor*>(d->tabWidget->widget(i));
        s.setArrayIndex(i);
        s.setValue(QLatin1String("tab"), tab->saveState());
    }
    s.endArray();
}

AbstractEditor * BrowserWindow::currentEditor() const
{
    return currentTab();
}

BrowserWindow * BrowserWindow::currentWindow()
{
    return qobject_cast<BrowserWindow *>(qApp->activeWindow());
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

void BrowserWindow::back()
{
    currentTab()->history()->back();
}

void BrowserWindow::forward()
{
    currentTab()->history()->forward();
}

void BrowserWindow::up()
{
    QUrl url = currentTab()->url();
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

void BrowserWindow::open(const QUrl &url)
{
    Q_D(BrowserWindow);

    if (d->tabWidget->count() == 0)
        openNewTab(url);
    else
        currentTab()->open(url);

    d->updateUi(currentTab());

    d->upAction->setEnabled(!(url.path().isEmpty() || url.path() == "/"));
}

void BrowserWindow::openEditor(const QString &id)
{
    Q_D(BrowserWindow);

    QUrl url;
    url.setScheme(qApp->applicationName());
    url.setHost(id);
    if (d->tabWidget->count() == 0) {
        int index = -1;
        StackedEditor *tab = d->addTab(&index);
        tab->open(url);
        d->tabWidget->setCurrentIndex(index);

//        if (!tab->currentEditor())
//            closeTab(index); // close tab or window if no editor found
    } else {
        currentTab()->open(url);
    }

    d->updateUi(currentTab());
}

void BrowserWindow::openNewTab(const QUrl &url)
{
    Q_D(BrowserWindow);

    int index = -1;
    StackedEditor *tab = d->addTab(&index);
    tab->open(url);
    d->tabWidget->setCurrentIndex(index);

//    if (!tab->currentEditor())
//        closeTab(index); // close tab or window if no editor found
}

void BrowserWindow::openNewTab(const QList<QUrl> &urls)
{
    foreach (const QUrl & url, urls) {
        openNewTab(url);
    }
}

void BrowserWindow::openNewWindow(const QUrl &path)
{
    BrowserWindow *window = createWindow();
    window->open(path);
    window->show();
}

void BrowserWindow::openNewWindow(const QList<QUrl> &urls)
{
    BrowserWindow *window = createWindow();
    foreach (const QUrl & url, urls) {
        window->openNewTab(url);
    }
    window->show();
}

void BrowserWindow::newTab()
{
    openNewTab(QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)));
}

void BrowserWindow::newWindow()
{
    openNewWindow(QUrl::fromLocalFile(QDesktopServices::storageLocation(QDesktopServices::HomeLocation)));
}

void BrowserWindow::closeTab(int index)
{
    Q_D(BrowserWindow);

    if (d->tabWidget->count() <= 1) {
        close();
        return;
    }

    if (index == -1) {
        index = d->tabWidget->currentIndex();
    }

    if (index == d->tabWidget->currentIndex())
        d->tabWidget->setCurrentIndex(index ? index - 1 : d->tabWidget->count() - 1); // switch to other tab before closing to prevent losing focus

    QWidget *widget = d->tabWidget->widget(index);
    d->tabWidget->removeTab(index);
    widget->deleteLater();
}

void BrowserWindow::save()
{
    if (!(currentEditor()->capabilities() & AbstractEditor::CanSave))
        return;

    currentEditor()->save();
}

void BrowserWindow::saveAs()
{
    // TODO: remember previous dir and set filename using title + extension from mimetype
    QString path = QFileDialog::getSaveFileName(this, tr("Save as"));

    if (path.isEmpty())
        return;

    if (!(currentEditor()->capabilities() & AbstractEditor::CanSave))
        return;

    currentEditor()->save(QUrl::fromLocalFile(path));
}

void BrowserWindow::refresh()
{
    AbstractEditor *e = currentEditor();
    if (e)
        e->refresh();
}

void BrowserWindow::cancel()
{
    AbstractEditor *e = currentEditor();
    if (e)
        e->cancel();
}

void BrowserWindow::nextTab()
{
    Q_D(BrowserWindow);

    int index = d->tabWidget->currentIndex();
    d->tabWidget->setCurrentIndex(index == d->tabWidget->count() - 1 ? 0 : index + 1);
}

void BrowserWindow::prevTab()
{
    Q_D(BrowserWindow);

    int index = d->tabWidget->currentIndex();
    d->tabWidget->setCurrentIndex(index ? index - 1 : d->tabWidget->count() - 1);
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
