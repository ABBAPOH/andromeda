#ifndef FILEMANAGERPLUGIN_H
#define FILEMANAGERPLUGIN_H

#include <ExtensionSystem/IPlugin>

#if QT_VERSION >= 0x050000
#include <QtCore/QStandardPaths>
#else
#include <IO/QStandardPaths>
#endif

#include <QtGui/QIcon>
#include <QtGui/QKeySequence>

#include <FileManager/NavigationModel>

class QSignalMapper;
class QSettings;

namespace Parts {
class SharedProperties;
}

namespace FileManager {

class FileManagerSettings;
class NavigationPanelSettings;

class FileManagerPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.arch.Andromeda.FileManagerPlugin")
#endif
public:
    explicit FileManagerPlugin();
    ~FileManagerPlugin();

    bool initialize();
    void shutdown();

    static FileManagerPlugin *instance();
    Parts::SharedProperties *properties() const;

private slots:
    void goTo(const QString &s);
    void onPathsDropped(const QString &destination, const QStringList &paths, Qt::DropAction action);

private:
    void createActions();
    void createSortByMenu();
    void createGoToMenu();
    void createGoToDirCommand(QStandardPaths::StandardLocation location,
                              const QIcon &icon = QIcon(),
                              const QKeySequence &key = QKeySequence());
    void connectGoToActions();

    void loadSettings();
    void saveSettings();

private:
    QSignalMapper *gotoMapper;

    Parts::SharedProperties *m_properties;
    FileManager::FileManagerSettings *m_fileManagerSettings;
    FileManager::NavigationPanelSettings *m_panelSettings;
};

} // namespace FileManager

#endif // FILEMANAGERPLUGIN_H
