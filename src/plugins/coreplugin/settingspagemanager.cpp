#include "settingspagemanager.h"

#include <QtCore/QMap>

#include "isettingspage.h"

namespace CorePlugin {

struct Category
{
    QString id;
    QList<ISettingsPage *> pages;
};

class SettingsPageManagerPrivate
{
public:
    QMap<QString, Category *> categories;
    QMap<QString, ISettingsPage *> pages;
};

} // namespace CorePlugin

using namespace CorePlugin;

/*!
    \class SettingsPageManager

    \brief The SettingsPageManager class contains implementations of ISettingsPage interface used in SettingsDialog
*/

/*!
    \brief Creates a SettingsPageManager with the given \a parent.
*/
SettingsPageManager::SettingsPageManager(QObject *parent) :
    QObject(parent),
    d_ptr(new SettingsPageManagerPrivate)
{
    setObjectName(QLatin1String("SettingsPageManager"));
}

/*!
    \brief Destroys SettingsPageManager.
*/
SettingsPageManager::~SettingsPageManager()
{
    Q_D(SettingsPageManager);

    qDeleteAll(d->categories);

    delete d_ptr;
}

/*!
    \brief Returns previously added page with \a id.
*/
ISettingsPage * SettingsPageManager::page(const QString &id) const
{
    return d_func()->pages.value(id);
}

/*!
    \brief Adds new page to this manager.
*/
void SettingsPageManager::addPage(ISettingsPage *page)
{
    Q_D(SettingsPageManager);

    if (!page)
        return;

    if (d->pages.contains(page->id()))
        return;

    QString categoryId = page->category();
//    if (!categoryEsists(categoryId))
//        addCategory(categoryId, page->displayName(), page->displayIcon());
    Category *c = d->categories.value(categoryId);
    if (!c) {
        c = new Category;
        c->id = categoryId;
        d->categories.insert(categoryId, c);
    }

    c->pages.append(page);
    d->pages.insert(page->id(), page);

    connect(page, SIGNAL(destroyed(QObject*)), this, SLOT(onDestroyed(QObject*)));
    emit pageAdded(page);
}

/*!
    \brief Removes page from this manager.
*/
void SettingsPageManager::removePage(ISettingsPage *page)
{
    Q_D(SettingsPageManager);

    if (!page)
        return;

    if (!d->pages.contains(page->id()))
        return;

    foreach (Category *c, d->categories) {
        c->pages.removeAll(page);
        if (c->pages.isEmpty()) {
            d->categories.remove(c->id);
            delete c;
        }
    }
    d->pages.remove(page->id());

    disconnect(page, 0, this, 0);
    emit pageRemoved(page);
}

/*!
    \brief Returns list of all categories in this manager.
*/
QStringList SettingsPageManager::categories() const
{
    return d_func()->categories.keys();
}

/*!
    \brief Returns list of all pages added to this manager.
*/
QList<ISettingsPage *> SettingsPageManager::pages() const
{
    QList<ISettingsPage *> result;

    foreach (const QString &category, categories()) {
        result.append(pages(category));
    }

    return result;
}

/*!
    \brief Returns list of all pages with category \a category.
*/
QList<ISettingsPage *> SettingsPageManager::pages(const QString &category) const
{
    Category *c = d_func()->categories.value(category);
    if (c)
        return c->pages;

    return QList<ISettingsPage*>();
}

/*!
    \internal
*/
void SettingsPageManager::onDestroyed(QObject *o)
{
    ISettingsPage *page = static_cast<ISettingsPage*>(o);
    removePage(page);
}
