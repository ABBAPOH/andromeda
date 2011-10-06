#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <iplugin.h>
#include "mainwindow.h"

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();
    ~CorePluginImpl();

    bool initialize(const QVariantMap &);
    void shutdown();

public slots:
    void newWindow();
    void showPluginView();
    void prefenrences();

    void handleMessage(const QString &);

    void restoreSession();
    void saveSession();

    void quit();

    void about();
    void aboutQt();

private:
    void createActions();
    void createFileMenu();
    void createEditMenu();
    void createViewMenu();
    void createGoToMenu();
    void createToolsMenu();
    void createHelpMenu();

private:
    QStringList urls;
};

#endif // COREPLUGIN_H
