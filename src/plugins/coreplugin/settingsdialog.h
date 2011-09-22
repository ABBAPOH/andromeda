#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "coreplugin_global.h"

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>

namespace CorePlugin {

class ISettingsPage;
class SettingsPageManager;

class Category;
class CategoryModel;

class SettingsDialogPrivate;
class COREPLUGIN_EXPORT SettingsDialog : public QDialog
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
    void onPageAdded(ISettingsPage *page);
    void onPageRemoved(ISettingsPage *page);

protected:
    SettingsDialogPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // SETTINGSDIALOG_H
