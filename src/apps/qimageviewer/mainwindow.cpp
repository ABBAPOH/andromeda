#include "mainwindow.h"

#include <QDebug>
#include <QPointer>

#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>

#include <qimageview/qimageview.h>
#include <qimageview/qimageviewresizedialog.h>

#include <widgets/windowsmenu.h>

#include "preferenceswindow.h"

QSize m_lastSize;
QList<MainWindow*> m_windows;

static const qint32 m_magic = 0x6d303877; // "m08w"
static const qint8 m_version = 1;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi();

    QAction *easterEggAction = new QAction(this);
    easterEggAction->setShortcuts(QList<QKeySequence>() << QKeySequence("Shift+Space"));
    connect(easterEggAction, SIGNAL(triggered()), SLOT(easterEgg()));
    addAction(easterEggAction);

    setupConnections();

    if (!m_lastSize.isEmpty())
        resize(m_lastSize);
    else
        resize(800, 600);
//    view->setImage(QImage("/Users/arch/Pictures/2048px-Smiley.svg.png"));
    retranslateUi();

    if (!m_windows.isEmpty()) {
        MainWindow *last = m_windows.last();
        move(last->pos() + QPoint(20, 20));
    }
}

MainWindow::~MainWindow()
{
}

QList<MainWindow*> MainWindow::windows()
{
    return m_windows;
}

QByteArray MainWindow::saveState() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);

    s << m_magic;
    s << m_version;
    s << saveGeometry();
    s << QMainWindow::saveState();
    s << m_file;
    s << view->saveState();

    return result;
}

bool MainWindow::restoreState(const QByteArray &arr)
{
    QByteArray state(arr);
    QDataStream s(&state, QIODevice::ReadOnly);

    QByteArray windowState, windowGeometry, viewState;

    qint32 magic;
    qint8 version;

    s >> magic;
    if (magic != m_magic)
        return false;

    s >> version;
    if (version != m_version)
        return false;

    s >> windowGeometry;
    s >> windowState;
    s >> m_file;
    s >> viewState;

    bool ok = true;
    if (ok)
        ok |= QMainWindow::restoreGeometry(windowGeometry);
    if (ok)
        ok |= QMainWindow::restoreState(windowState);
    if (ok)
        ok |= view->restoreState(viewState);

    updateTitle();

    return ok;
}

void MainWindow::about()
{
    QPixmap pixmap(":/icons/qimageviewer.png");
    pixmap = pixmap.scaled(64, 64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("About"));
    msgBox.setText(tr("QImageViewer"));
    msgBox.setInformativeText(tr("(c) Ivan Komissarov\n\ne-mail: ABBAPOH@gmail.com"));
    msgBox.setIconPixmap(pixmap);
    msgBox.exec();
}

void MainWindow::open()
{
    QStringList files = QFileDialog::getOpenFileNames(this);
    if (files.isEmpty())
        return;

    if (view->image().isNull()) {
        open(files.first());
        files = files.mid(1);
        if (!files.isEmpty())
            openWindow(files);
    } else {
        openWindow(files);
    }
}

void MainWindow::open(const QString &file)
{
    m_file = file;
    QFile *f = new QFile(file);
    if (!f->open(QFile::ReadOnly))
        qWarning() << "Can't open file" << file;
    view->read(f);

    updateTitle();
}

void MainWindow::openWindow(const QString &file)
{
    MainWindow *window = new MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
    window->open(file);
}

void MainWindow::openWindow(const QStringList &files)
{
    if (files.count() > 10) {
        QMessageBox msgBox;
        msgBox.setWindowTitle(tr("Open"));
        msgBox.setText(tr("You are opening %1 files. Would you like to continue?").arg(files.count()));
        msgBox.setInformativeText(tr("Opening large amount of files can cause drop in performance"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        if (msgBox.exec() == QMessageBox::No)
            return;
    }

    foreach (const QString &file, files) {
        openWindow(file);
    }
}

void MainWindow::newWindow()
{
    MainWindow *window = new MainWindow;
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}

void MainWindow::save()
{
    QFile f(m_file);
    view->write(&f, QFileInfo(m_file).suffix().toUtf8());
//    view->image().save(m_file);
    view->setModified(false);
}

void MainWindow::saveAs()
{
    QString file = QFileDialog::getSaveFileName(this);
    if (file.isEmpty())
        return;

    m_file = file;
    save();
    updateTitle();
}

void MainWindow::preferences()
{
    static QPointer<PreferencesWindow> widget;

    if (!widget) {
        widget = new PreferencesWindow();
        widget->setAttribute(Qt::WA_DeleteOnClose);
        widget->show();
    } else {
        widget->show();
        widget->raise();
        widget->activateWindow();
    }
}

void MainWindow::resizeImage()
{
    QImageViewResizeDialog d(this);
    d.setImageSize(view->image().size());
    if (d.exec()) {
        view->resizeImage(d.imageSize());
    }
}

void MainWindow::easterEgg()
{
    if (view->image().isNull()) {
        open(":/icons/qimageviewer.png");
    } else {
        openWindow(":/icons/qimageviewer.png");
    }
}

void MainWindow::updateSaveActions()
{
    bool canSaveAs = view->canWrite();
    bool canSave = canSaveAs && view->isModified();

    actionSave->setEnabled(canSave);
    actionSaveAs->setEnabled(canSaveAs);

    actionResize->setEnabled(!view->image().isNull());

    updateTitle();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    // TODO: add auto saving on quit
    if (!view->isModified())
        return;

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Save"));
    msgBox.setText(tr("File has been modified. Would you like to save it?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard);
    msgBox.setWindowModality(Qt::WindowModal);
    int button = msgBox.exec();

    switch (button) {
    case QMessageBox::Save: {
        if (m_file.isEmpty())
            saveAs();
        else
            save();

        if (view->isModified())
            e->ignore();
        else
            e->accept();
        break;
    }
    case QMessageBox::Cancel: {
        e->ignore();
        break;
    }
    case QMessageBox::Discard: {
        e->accept();
        break;
    }
    default:
        break;
    }
}

void MainWindow::hideEvent(QHideEvent *)
{
    m_windows.removeOne(this);
}

void MainWindow::showEvent(QShowEvent *)
{
    m_windows.prepend(this);
}

void MainWindow::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            m_windows.removeOne(this);
            m_windows.append(this);
        }
    }
    QMainWindow::changeEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    m_lastSize = e->size();
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("QImageViewer"));
#ifndef Q_OS_MAC
    setWindowIcon(QIcon(":/icons/qimageviewer.png"));
#endif

    view = new QImageView(this);
    setCentralWidget(view);

    setupMenuBar();
    setupToolBar();
}

void MainWindow::setupMenuBar()
{
    m_menuBar = new QMenuBar(this);
    this->setMenuBar(m_menuBar);

    setupFileMenu();
    setupEditMenu();
    setupViewMenu();
    setupToolsMenu();
    setupHelpMenu();
    m_menuBar->insertMenu(helpMenu->menuAction(), new WindowsMenu(m_menuBar));
}

void MainWindow::setupToolBar()
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setObjectName("toolBar");
    m_toolBar->setFloatable(false);
    m_toolBar->setMovable(false);

    m_toolBar->addAction(view->action(QImageView::ZoomIn));
    m_toolBar->addAction(view->action(QImageView::ZoomOut));
    m_toolBar->addSeparator();
    m_toolBar->addAction(view->action(QImageView::MoveTool));
    m_toolBar->addAction(view->action(QImageView::SelectionTool));
    m_toolBar->addSeparator();
    m_toolBar->addAction(view->action(QImageView::RotateLeft));
    m_toolBar->addAction(view->action(QImageView::RotateRight));

    setUnifiedTitleAndToolBarOnMac(true);
    addToolBar(m_toolBar);
}

void MainWindow::setupFileMenu()
{
    fileMenu = new QMenu(this);

    actionOpen = new QAction(this);
    actionOpen->setObjectName("actionOpen");
    actionOpen->setShortcut(QKeySequence::Open);
    fileMenu->addAction(actionOpen);

    actionSave = new QAction(this);
    actionSave->setObjectName("actionSave");
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(false);
    fileMenu->addAction(actionSave);

    actionSaveAs = new QAction(this);
    actionSaveAs->setObjectName("actionSaveAs");
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionSaveAs->setEnabled(false);
    fileMenu->addAction(actionSaveAs);

    fileMenu->addSeparator();

    actionClose = new QAction(this);
    actionClose->setObjectName("actionClose");
    actionClose->setShortcut(QKeySequence("Ctrl+W"));
    fileMenu->addAction(actionClose);

    fileMenu->addSeparator();

    actionQuit = new QAction(this);
    actionQuit->setObjectName("actionQuit");
    actionQuit->setMenuRole(QAction::QuitRole);
    actionQuit->setShortcut(QKeySequence("Ctrl+Q"));
    fileMenu->addAction(actionQuit);

    m_menuBar->addMenu(fileMenu);
}

void MainWindow::setupEditMenu()
{
    editMenu = new QMenu(this);

    editMenu->addAction(view->action(QImageView::Undo));
    editMenu->addAction(view->action(QImageView::Redo));
    editMenu->addSeparator();
    editMenu->addAction(view->action(QImageView::Cut));
    editMenu->addAction(view->action(QImageView::Copy));
    editMenu->addSeparator();
    editMenu->addAction(view->action(QImageView::MoveTool));
    editMenu->addAction(view->action(QImageView::SelectionTool));
    editMenu->addSeparator();

    actionPreferences = new QAction(this);
    actionPreferences->setObjectName("actionPreferences");
    actionPreferences->setShortcut(QKeySequence("Ctrl+,"));
    actionPreferences->setMenuRole(QAction::PreferencesRole);
    editMenu->addAction(actionPreferences);

    m_menuBar->addMenu(editMenu);
}

void MainWindow::setupViewMenu()
{
    viewMenu = new QMenu(this);

    viewMenu->addAction(view->action(QImageView::ZoomIn));
    viewMenu->addAction(view->action(QImageView::ZoomOut));
    viewMenu->addAction(view->action(QImageView::FitInView));
    viewMenu->addAction(view->action(QImageView::NormalSize));

    m_menuBar->addMenu(viewMenu);
}

void MainWindow::setupToolsMenu()
{
    toolsMenu = new QMenu(this);

    actionResize = new QAction(this);
    actionResize->setObjectName("actionResize");
    actionResize->setEnabled(false);
    toolsMenu->addAction(actionResize);

    toolsMenu->addSeparator();

    toolsMenu->addAction(view->action(QImageView::RotateLeft));
    toolsMenu->addAction(view->action(QImageView::RotateRight));

    toolsMenu->addSeparator();

    toolsMenu->addAction(view->action(QImageView::FlipHorizontally));
    toolsMenu->addAction(view->action(QImageView::FlipVertically));

    toolsMenu->addSeparator();
    toolsMenu->addAction(view->action(QImageView::ResetOriginal));

    m_menuBar->addMenu(toolsMenu);
}

void MainWindow::setupHelpMenu()
{
    helpMenu = new QMenu(this);

    actionAbout = new QAction(this);
    actionAbout->setObjectName("actionAbout");
    actionAbout->setMenuRole(QAction::AboutRole);
    helpMenu->addAction(actionAbout);

    actionAboutQt = new QAction(this);
    actionAboutQt->setObjectName("actionAboutQt");
    actionAboutQt->setMenuRole(QAction::AboutQtRole);
    helpMenu->addAction(actionAboutQt);

    m_menuBar->addMenu(helpMenu);
}

void MainWindow::setupConnections()
{
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(actionClose, SIGNAL(triggered()), this, SLOT(close()));
    connect(actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

    connect(view, SIGNAL(modifiedChanged(bool)), this, SLOT(updateSaveActions()));
    connect(view, SIGNAL(canWriteChanged(bool)), this, SLOT(updateSaveActions()));
    connect(view, SIGNAL(canWriteChanged(bool)), this, SLOT(updateSaveActions()));

    connect(actionPreferences, SIGNAL(triggered(bool)), this, SLOT(preferences()));

    connect(actionResize, SIGNAL(triggered()), this, SLOT(resizeImage()));

    connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::retranslateUi()
{
    fileMenu->setTitle(tr("File"));
    editMenu->setTitle(tr("Edit"));
    viewMenu->setTitle(tr("View"));
    toolsMenu->setTitle(tr("Tools"));
    helpMenu->setTitle(tr("Help"));

    actionOpen->setText(tr("Open"));
    actionSave->setText(tr("Save"));
    actionSaveAs->setText(tr("Save as..."));
    actionClose->setText(tr("Close"));
    actionQuit->setText(tr("Quit"));
    actionPreferences->setText(tr("Preferences"));
    actionResize->setText(tr("Resize image..."));
    actionAbout->setText(tr("About..."));
    actionAboutQt->setText(tr("About Qt..."));
}

void MainWindow::updateTitle()
{
    bool modified = view->isModified();

    if (!m_file.isEmpty()) {
        if (modified)
            setWindowTitle(tr("%1* - QImageViewer").arg(QFileInfo(m_file).baseName()));
        else
            setWindowTitle(tr("%1 - QImageViewer").arg(QFileInfo(m_file).baseName()));
    } else {
        setWindowTitle(tr("QImageViewer"));
    }
}
