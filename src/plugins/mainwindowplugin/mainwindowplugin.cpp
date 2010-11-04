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
using namespace IO;
bool MainWindowPluginImpl::initialize()
{
    FileCopyDialogManager *man = new FileCopyDialogManager;
    FileCopyManager *manager  = FileCopyManager::instance();
    QtFileCopier *copier = manager->copier();
    qDebug() << "mkdir" << QDir("/Users/arch").mkdir("1");
    copier->copyDirectory("/Users/arch/Sites", "/Users/arch/1", QtFileCopier::NonInteractive);

    qDebug("MainWindowPluginImpl::initialize");
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
