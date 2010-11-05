#include "mainwindowplugin.h"

#include <QtCore/QtPlugin>

using namespace MainWindowPlugin;

MainWindowPluginImpl::MainWindowPluginImpl() :
    IPlugin()
{
}

#include "filecopydialogmanager.h"
#include <filecopymanager.h>
#include <QDebug>
#include <QDir>
#include "filecopydialog.h"
using namespace IO;
bool MainWindowPluginImpl::initialize()
{
    FileCopyDialogManager *man = new FileCopyDialogManager;
    FileCopyManager *manager  = FileCopyManager::instance();
    QtFileCopier *copier = manager->copier();
    qDebug() << "mkdir" << QDir("/Users/arch").mkdir("1");
    copier->copyDirectory("/Users/arch/Sites", "/Users/arch/1");

    qDebug("MainWindowPluginImpl::initialize");
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
