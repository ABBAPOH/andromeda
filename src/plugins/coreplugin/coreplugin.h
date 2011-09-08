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

    bool initialize();
    void shutdown();

public slots:
    void newWindow();
    void showPluginView();
    void handleMessage(const QString &);

    void restoreSession();
    void saveSession();

    void quit();

    void about();
    void aboutQt();

private:
    void createActions();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    QList<CorePlugin::MainWindow *> m_windows;
};

#endif // COREPLUGIN_H
