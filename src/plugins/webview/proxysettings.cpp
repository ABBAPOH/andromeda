#include "proxysettings.h"
#include "ui_proxysettings.h"

#include <QtCore/QSettings>
#include <QtGui/QPushButton>

#include "webviewplugin.h"

ProxySettingsWidget::ProxySettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProxySettingsWidget),
    m_settings(new QSettings(this))
{
    ui->setupUi(this);

    m_settings->beginGroup(QLatin1String("webview/proxy"));

    loadSettings();

    connect(ui->proxySupport, SIGNAL(toggled(bool)), SLOT(makeDirty()));
    connect(ui->proxyType, SIGNAL(currentIndexChanged(int)), SLOT(makeDirty()));
    connect(ui->proxyHostName, SIGNAL(textChanged(QString)), SLOT(makeDirty()));
    connect(ui->proxyPort, SIGNAL(valueChanged(int)), SLOT(makeDirty()));
    connect(ui->proxyUserName, SIGNAL(textChanged(QString)), SLOT(makeDirty()));
    connect(ui->proxyPassword, SIGNAL(textChanged(QString)), SLOT(makeDirty()));

    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), SLOT(onClick(QAbstractButton*)));
}

ProxySettingsWidget::~ProxySettingsWidget()
{
    delete ui;
}

void ProxySettingsWidget::onClick(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);

    if (role == QDialogButtonBox::ApplyRole)
        saveSettings();
    else if (role == QDialogButtonBox::ResetRole)
        loadSettings();
}

void ProxySettingsWidget::makeDirty()
{
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(true);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
}

void ProxySettingsWidget::loadSettings()
{
    ui->proxySupport->setChecked(m_settings->value(QLatin1String("enabled"), false).toBool());
    ui->proxyType->setCurrentIndex(m_settings->value(QLatin1String("type"), 0).toInt());
    ui->proxyHostName->setText(m_settings->value(QLatin1String("hostName")).toString());
    ui->proxyPort->setValue(m_settings->value(QLatin1String("port"), 1080).toInt());
    ui->proxyUserName->setText(m_settings->value(QLatin1String("userName")).toString());
    ui->proxyPassword->setText(m_settings->value(QLatin1String("password")).toString());

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
}

void ProxySettingsWidget::saveSettings()
{
    m_settings->setValue(QLatin1String("enabled"), ui->proxySupport->isChecked());
    m_settings->setValue(QLatin1String("type"), ui->proxyType->currentIndex());
    m_settings->setValue(QLatin1String("hostName"), ui->proxyHostName->text());
    m_settings->setValue(QLatin1String("port"), ui->proxyPort->text());
    m_settings->setValue(QLatin1String("userName"), ui->proxyUserName->text());
    m_settings->setValue(QLatin1String("password"), ui->proxyPassword->text());
    m_settings->sync();

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);

    WebViewPlugin::instance()->loadProxySettings();
}

ProxySettingsPage::ProxySettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("Proxy"), QLatin1String("WebView"), parent)
{
}

QWidget *ProxySettingsPage::createPage(QWidget *parent)
{
    return new ProxySettingsWidget(parent);
}
