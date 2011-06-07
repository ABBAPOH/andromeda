#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtGui/QDialog>
#include <QtGui/QStackedLayout>
#include <QtGui/QListView>
#include <QtGui/QLabel>

namespace CorePlugin {

class Category;
class CategoryModel;
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = 0);

    CategoryModel *model();
    void setModel(CategoryModel *model);

signals:

private slots:
    void currentChanged(const QModelIndex &current);

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
};

} // namespace CorePlugin

#endif // SETTINGSDIALOG_H
