#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <extensionsystem/iplugin.h>
#include "browserwindow.h"

using namespace ExtensionSystem;

class CorePluginImpl : public IPlugin
{
    Q_OBJECT
public:
    CorePluginImpl();
    ~CorePluginImpl();

    bool initialize(const QVariantMap &);
    void shutdown();

    bool restoreState(const QByteArray &state);
    QByteArray saveState() const;

public slots:
    void newWindow();
    void showPluginView();
    void showSettings();
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
    void registerAtions();

    void createAction(const QByteArray &id, const char *slot);

private:
    QStringList urls;
};

#endif // COREPLUGIN_H
