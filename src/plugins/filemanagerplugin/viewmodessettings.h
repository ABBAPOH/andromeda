#ifndef VIEWMODESSETTINGS_H
#define VIEWMODESSETTINGS_H

#include <QtCore/QString>

#include <QtGui/QWidget>
#include <QtGui/QIcon>

#include <GuiSystem/SettingsPage>

class QSettings;

namespace Ui {
    class ViewModesSettingsWidget;
}

namespace GuiSystem {
class SharedProperties;
}

namespace FileManager {

class FileManagerSettings;
class NavigationPanelSettings;

class ViewModesSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewModesSettingsWidget(QWidget *parent = 0);
    ~ViewModesSettingsWidget();

private slots:
    void onChecked(bool);
    void onIconSizeChanged(int);
    void onGridSizeChanged(int);
    void onFlowChanged(int);
    void onColumnIconSizeChanged(int);
    void onTreeIconSizeChanged(int);
    void onItemsExpandableChecked(bool);

private:
    void setupLeftPanel();
    void setupIconSize();
    void setupGridSize();
    void setupFlow();
    void setupTreeView();

private:
    Ui::ViewModesSettingsWidget *ui;

    GuiSystem::SharedProperties *m_properties;
    FileManager::FileManagerSettings *m_fileManagerSettings;
    FileManager::NavigationPanelSettings *m_panelSettings;
};

class ViewModesSettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit ViewModesSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

} // namespace FileManager

#endif // VIEWMODESSETTINGS_H
