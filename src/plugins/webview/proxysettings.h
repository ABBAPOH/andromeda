#ifndef PROXYSETTINGS_H
#define PROXYSETTINGS_H

#include <QtGui/QWidget>

class QAbstractButton;
class QSettings;

namespace Ui {
class ProxySettingsWidget;
}

class ProxySettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProxySettingsWidget(QWidget *parent = 0);
    ~ProxySettingsWidget();

private slots:
    void onClick(QAbstractButton*);
    void makeDirty();

private:
    void loadSettings();
    void saveSettings();

private:
    Ui::ProxySettingsWidget *ui;
    QSettings *m_settings;
};

#include <guisystem/settingspage.h>

class ProxySettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit ProxySettingsPage(QObject *parent = 0);

    QString name() const { return tr("Proxy"); }
    QIcon icon() const { return QIcon(); }

    QString categoryName() const { return tr("Web view"); }
    QIcon categoryIcon() const { return QIcon(); }

    QWidget *createPage(QWidget *parent);
};

#endif // PROXYSETTINGS_H
