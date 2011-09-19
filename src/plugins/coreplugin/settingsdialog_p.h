#ifndef SETTINGSDIALOG_P_H
#define SETTINGSDIALOG_P_H

#include "settingsdialog.h"

#include <QtCore/QAbstractItemModel>
#include <QtGui/QIcon>

#include "isettingspage.h"

class QTabWidget;

namespace CorePlugin {

class Category {
public:
    QString id;
    QString displayName() { return pages.isEmpty() ? QString() : pages.first()->displayCategory(); }
    QIcon icon() const { return pages.isEmpty() ? QIcon() : pages.first()->displayIcon(); }
    QList<ISettingsPage *> pages;
    int index;
};

class CategoryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CategoryModel(QObject *parent = 0);
    ~CategoryModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    void addPage(ISettingsPage* page);
    void removePage(ISettingsPage* page);

    const QList<Category*> &categories() const { return m_categories; }
    Category *findCategoryById(const QString &id);

signals:
    void pageAdded(ISettingsPage* page);
    void pageRemoved(ISettingsPage* page);

private:
    QList<Category*> m_categories;
    QIcon m_emptyIcon;
};

} // namespace CorePlugin

#endif // SETTINGSDIALOG_P_H
