#ifndef SETTINGSPAGEMANAGER_H
#define SETTINGSPAGEMANAGER_H

#include <QtCore/QObject>
#include <QtGui/QIcon>

namespace CorePlugin {

class ISettingsPage;

class SettingsPageManagerPrivate;
class SettingsPageManager : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(SettingsPageManager)
    Q_DISABLE_COPY(SettingsPageManager)

public:
    explicit SettingsPageManager(QObject *parent = 0);
    ~SettingsPageManager();

    ISettingsPage *page(const QString &id) const;
    void addPage(ISettingsPage* page);
    void removePage(ISettingsPage* page);

    QStringList categories() const;

    QList<ISettingsPage *> pages() const;
    QList<ISettingsPage *> pages(const QString &category) const;

signals:
    void pageAdded(ISettingsPage *);
    void pageRemoved(ISettingsPage *);

private slots:
    void onDestroyed(QObject *o);

protected:
    SettingsPageManagerPrivate *d_ptr;
};

} // namespace CorePlugin

#endif // SETTINGSPAGEMANAGER_H
