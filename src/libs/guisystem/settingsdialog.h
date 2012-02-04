#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "guisystem_global.h"

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>

namespace GuiSystem {

class SettingsPage;
class SettingsPageManager;

class Category;
class CategoryModel;

class SettingsDialogPrivate;
class GUISYSTEM_EXPORT SettingsDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsDialog)
    Q_DISABLE_COPY(SettingsDialog)

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    SettingsPageManager *settingsPageManager() const;
    void setSettingsPageManager(SettingsPageManager *manager);

private slots:
    void currentChanged(const QModelIndex &current);
    void onPageAdded(SettingsPage *page);
    void onPageRemoved(SettingsPage *page);

protected:
    SettingsDialogPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // SETTINGSDIALOG_H
