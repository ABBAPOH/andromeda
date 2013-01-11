#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <GuiSystem/SettingsPage>
#include <QtCore/QString>
#include <QtGui/QIcon>
#include <QtGui/QWidget>

class QSettings;

namespace Ui {
class GlobalSettingsWidget;
}

namespace FileManager {

class FileManagerSettings;
class NavigationPanelSettings;

class GlobalSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalSettingsWidget(QWidget *parent = 0);
    ~GlobalSettingsWidget();

private slots:
    void onFileRemoveToggled(bool);
    void onExtensionChangeToggled(bool);

private:
    Ui::GlobalSettingsWidget *ui;

    QSettings *m_settings;
    FileManager::FileManagerSettings *m_fileManagerSettings;
};

class GlobalSettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit GlobalSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace FileManager

#endif // GLOBALSETTINGS_H
