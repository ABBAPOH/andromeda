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

class SettingsDialogPrivate;
class GUISYSTEM_EXPORT SettingsDialog : public QMainWindow
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsDialog)
    Q_DISABLE_COPY(SettingsDialog)

public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    SettingsPageManager *settingsPageManager() const;
    void setSettingsPageManager(SettingsPageManager *manager);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

private slots:
    void onSelectionChanged(const QItemSelection &current);
    void onPageAdded(SettingsPage *page);
    void onPageRemoved(SettingsPage *page);
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

protected:
    SettingsDialogPrivate *d_ptr;
};

} // namespace GuiSystem

#endif // SETTINGSDIALOG_H
