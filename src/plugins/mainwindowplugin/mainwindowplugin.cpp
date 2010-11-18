#include "mainwindowplugin.h"

#include <QtCore/QtPlugin>

using namespace MainWindowPlugin;

MainWindowPluginImpl::MainWindowPluginImpl() :
    IPlugin()
{
}

#include "mainwindow.h"

#include "filecopydialogmanager.h"
#include <filecopymanager.h>
#include <QDebug>
#include <QDir>
#include "filecopydialog.h"
#include <QTreeView>
#include <filesystemmodel.h>
using namespace IO;
bool MainWindowPluginImpl::initialize()
{
    MainWindow *window = new MainWindow();
    window->show();
    return true;
    FileCopyDialogManager *man = new FileCopyDialogManager(this);
//    FileCopyManager *manager  = FileCopyManager::instance();
//    QtFileCopier *copier = manager->copier();
//    qDebug() << "mkdir" << QDir("/Users/arch").mkdir("1");
//    copier->copyDirectory("/Users/arch/Sites", "/Users/arch/1");

    QTreeView *view = new QTreeView;
    view->setAttribute(Qt::WA_DeleteOnClose);
    FileSystemModel *model = new FileSystemModel(view);
    model->setReadOnly(false);
    model->setRootPath("");
    view->setModel(model);
    view->show();
    view->setDragDropMode(QAbstractItemView::DragDrop);
    view->setDragEnabled(true);
    view->setAcceptDrops(true);
    view->setRootIndex(model->index("/Users/arch/"));
    view->resize(640, 480);
    view->setColumnWidth(0, 300);
    view->setSelectionMode(QAbstractItemView::ContiguousSelection);

    qDebug("MainWindowPluginImpl::initialize");
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
