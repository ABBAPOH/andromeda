#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "guisystem_global.h"

#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>

namespace GuiSystem {

class SettingsPage;
class SettingsPageManager;

class Category;
class CategoryModel;

class SettingsWindowPrivate;
class GUISYSTEM_EXPORT SettingsWindow : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsWindow)
    Q_DISABLE_COPY(SettingsWindow)

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();

    SettingsPageManager *settingsPageManager() const;
    void setSettingsPageManager(SettingsPageManager *manager);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

private slots:
    void onSelectionChanged(const QItemSelection &current);
    void onPageAdded(SettingsPage *page);
    void onPageRemoved(SettingsPage *page);
    void onActionTriggered(bool toggled);

protected:
    SettingsWindowPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // SETTINGSDIALOG_H
