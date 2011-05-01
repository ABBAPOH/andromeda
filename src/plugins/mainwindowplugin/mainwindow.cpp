#include "mainwindow.h"
#include "mainwindow_p.h"

#include <QtCore/QFileInfo>
#include <QtGui/QAction>
#include <QDebug>
#include <CorePlugin>

#include <iview.h>

using namespace MainWindowPlugin;
using namespace CorePlugin;
//using namespace GuiSystem;

QString getMimeType(const QString &path)
{
    QFileInfo info(path);
    if (info.isDir())
        return "inode/directory";
    return "";
}

void MainWindowPrivate::onTextEntered(const QString &path)
{
    Q_Q(MainWindow);

    QString mime = getMimeType(path);
    GuiSystem::GuiController *controller = GuiSystem::GuiController::instance();
    QString perspectiveId = Core::instance()->perspectiveManager()->perspective(mime);
    q->currentState()->setProperty("path", path);
    q->currentState()->setCurrentPerspective(perspectiveId);

    QString mainViewId = controller->perspective(perspectiveId)->property("MainView").toString();
    GuiSystem::IView *iview = q->currentState()->view(mainViewId);
    IHistoryView *view = qobject_cast<IHistoryView*>(iview);
    if (view) {
        currentHistory->appendView(view);
    } else {
        HistoryItem item(QIcon(), QDateTime::currentDateTime(), "", path);
        currentHistory->appendItem(item);
    }
    qDebug() << "MainWindowPrivate::onTextEntered" << path << mime << perspectiveId;
    // TODO: think disconnect
    connect(q->currentState(), SIGNAL(propertyChanged(const char*,QVariant)),
            SLOT(onPropertyChanged(const char*,QVariant)));
}

void MainWindowPrivate::onPropertyChanged(const char *name, const QVariant &value)
{
    if (QLatin1String(name) == QLatin1String("path")) {
        qDebug() << name;
        lineEdit->setText(value.toString());
    }
}

MainWindow::MainWindow(QWidget *parent) :
    GuiSystem::MainWindow(parent),
    d_ptr(new MainWindowPrivate(this))
{
    Q_D(MainWindow);

    d->currentHistory = new CorePlugin::GlobalHistory(currentState());

    d->lineEdit = new EnteredLineEdit(this);
    d->toolBar = new QToolBar(this);

    QAction *backAction = new QAction(QIcon(":/images/icons/back.png"), "Back", this);
    QAction *forwardAction = new QAction(QIcon(":/images/icons/forward.png"), "Forward", this);

    d->toolBar->addAction(backAction);
    d->toolBar->addAction(forwardAction);
    d->toolBar->addSeparator();
    d->toolBar->addWidget(d->lineEdit);

    addToolBar(d->toolBar);
    setUnifiedTitleAndToolBarOnMac(true);

    connect(d->lineEdit, SIGNAL(textEntered(QString)), d, SLOT(onTextEntered(QString)));
    connect(backAction, SIGNAL(triggered()), SLOT(back()));
    connect(forwardAction, SIGNAL(triggered()), SLOT(forward()));

    resize(640, 480);
}

MainWindow::~MainWindow()
{
    delete d_ptr;
}

void MainWindow::back()
{
    Q_D(MainWindow);

    if (d->currentHistory->canGoBack()) {
        d->currentHistory->back();
        GlobalHistoryItem *item = d->currentHistory->currentItem();
        if (item->type == GlobalHistoryItem::View) {
            item->view->back();
        } else {

        }
    }

    QString mainViewId = currentState()->currentInstance()->perspective()->property("MainView").toString();
    GuiSystem::IView *iview = currentState()->view(mainViewId);
    CorePlugin::IHistoryView * view = qobject_cast<CorePlugin::IHistoryView *>(iview);
    view->back();
}

void MainWindow::forward()
{
    QString mainViewId = currentState()->currentInstance()->perspective()->property("MainView").toString();
    GuiSystem::IView *iview = currentState()->view(mainViewId);
    CorePlugin::IHistoryView * view = qobject_cast<CorePlugin::IHistoryView *>(iview);
    view->forward();
}
