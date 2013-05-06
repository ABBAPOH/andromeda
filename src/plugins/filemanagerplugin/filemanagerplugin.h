#ifndef FILEMANAGERPLUGIN_H
#define FILEMANAGERPLUGIN_H

#include <ExtensionSystem/IPlugin>

#include <QtGui/QDesktopServices>
#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

#include <FileManager/NavigationModel>

class QSignalMapper;
class QSettings;

namespace FileManager {

class FileManagerSettings;
class NavigationPanelSettings;

class FileManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

public:
    explicit FileManagerPlugin(QObject *parent = 0);

    bool initialize();
    void shutdown();

private slots:
    void goTo(const QString &s);
    void onPathsDropped(const QString &destination, const QStringList &paths, Qt::DropAction action);

private:
    void createActions();
    void createSortByMenu();
    void createGoToMenu();
    void createGoToDirCommand(QDesktopServices::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
    void connectGoToActions();
    void createPanesMenu();

    void loadSettings();
    void saveSettings();

private:
    QSignalMapper *gotoMapper;

    QSettings *m_settings;
    FileManager::FileManagerSettings *m_fileManagerSettings;
    FileManager::NavigationPanelSettings *m_panelSettings;
};

} // namespace FileManager

#endif // FILEMANAGERPLUGIN_H
