#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

#include <iplugin.h>

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();

    bool initialize();
    void shutdown();

public slots:
    void newWindow();
    void showPluginView();
    void handleMessage(const QString &);

private:
    void createActions();
    void createGotoDirCommand(QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
};

#endif // COREPLUGIN_H
