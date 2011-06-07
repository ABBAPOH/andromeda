#ifndef SETTINGSDIALOG_P_H
#define SETTINGSDIALOG_P_H

#include "settingsdialog.h"

#include <QtCore/QAbstractItemModel>
#include <QtGui/QIcon>

#include "ioptionspage.h"

class QTabWidget;

namespace CorePlugin {

class Category {
public:
    QString id;
    QString displayName() { return pages.isEmpty() ? QString() : pages.first()->displayCategory(); }
    QIcon icon() const { return pages.isEmpty() ? QIcon() : pages.first()->categoryIcon(); }
    QList<IOptionsPage *> pages;
    int index;
    QTabWidget *tabWidget;
};

class CategoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CategoryModel(QObject *parent = 0);
    ~CategoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addPage(IOptionsPage* page);
    void removePage(IOptionsPage* page);

    const QList<Category*> &categories() const { return m_categories; }

private:
    Category *findCategoryById(const QString &id);

    QList<Category*> m_categories;
    QIcon m_emptyIcon;
};

} // namespace CorePlugin

#endif // SETTINGSDIALOG_P_H
