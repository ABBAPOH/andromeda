#ifndef FILEMANAGERPLUGIN_H
#define FILEMANAGERPLUGIN_H

#include <extensionsystem/iplugin.h>

#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>
#include "navigationmodel.h"

class QSignalMapper;
class QSettings;

namespace FileManagerPlugin {
    class FileManagerSettings;
    class NavigationPanelSettings;
} // namespace FileManagerPlugin

class FileManagerPluginImpl : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit FileManagerPluginImpl(QObject *parent = 0);

    bool initialize(const QVariantMap &);
    void shutdown();

private slots:
    void goTo(const QString &s);

private:
    void createActions();
    void createFileMenu();
    void createViewMenu();
    void createGoToMenu();
    void createGoToDirCommand(QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
    void createGoToActions();
    void createSortByActons();

    void loadSettings();
    void saveSettings();

private:
    QSignalMapper *gotoMapper;

    QSettings *m_settings;
    FileManagerPlugin::FileManagerSettings *m_fileManagerSettings;
    FileManagerPlugin::NavigationPanelSettings *m_panelSettings;
};

#endif // FILEMANAGERPLUGIN_H
