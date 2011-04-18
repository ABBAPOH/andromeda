#include "mainwindowplugin.h"

#include <QtCore/QtPlugin>

using namespace MainWindowPlugin;

MainWindowPluginImpl::MainWindowPluginImpl() :
    IPlugin()
{
}

#include "mainwindow.h"

bool MainWindowPluginImpl::initialize()
{
    window = new MainWindow();
    window->show();

    return true;
}

void MainWindowPluginImpl::shutdown()
{
    delete window;
}

Q_EXPORT_PLUGIN(MainWindowPluginImpl)
