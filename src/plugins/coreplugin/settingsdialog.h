#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QStackedLayout>

namespace CorePlugin {

class IOptionsPage;
class Category;
class CategoryModel;
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    CategoryModel *model();
    void setModel(CategoryModel *model);

private slots:
    void currentChanged(const QModelIndex &current);
    void onPageRemoved(IOptionsPage *page);

private:
    void setupUi();
    void showCategory(int index);
    void ensureCategoryWidget(Category *category);

    CategoryModel *m_model;
    QString m_currentCategory;
    QString m_currentPage;
    QStackedLayout *m_stackedLayout;
    QListView *m_categoryList;
    QLabel *m_headerLabel;
    QMap<Category *, QTabWidget *> m_tabWidgets;
    QMap<IOptionsPage *, QWidget *> m_widgets;
};

} // namespace CorePlugin

#endif // SETTINGSDIALOG_H
