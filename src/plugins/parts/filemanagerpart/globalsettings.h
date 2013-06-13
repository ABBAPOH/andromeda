#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <QtCore/QString>

#include <QtGui/QIcon>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif

#include <Parts/SettingsPage>

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

class GlobalSettingsPage : public Parts::SettingsPage
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
