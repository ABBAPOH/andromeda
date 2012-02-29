#include "proxysettings.h"
#include "ui_proxysettings.h"

#include <QtCore/QSettings>
#include <QtGui/QPushButton>
#include <QNetworkProxy>

#include <QDebug>

#include "webviewplugin.h"

ProxySettingsWidget::ProxySettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProxySettingsWidget)
{
    ui->setupUi(this);

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
    QNetworkProxy proxy = QNetworkProxy::applicationProxy();

    if ( proxy.type() == QNetworkProxy::NoProxy )
        ui->proxySupport->setChecked(false);
    else
        ui->proxySupport->setChecked(true);

    int type = proxy.type();
    switch(type) {
    case 1:
        ui->proxyType->setCurrentIndex(0);
        break;
    case 3:
        ui->proxyType->setCurrentIndex(2);
        break;
    case 4:
        ui->proxyType->setCurrentIndex(1);
    }

    ui->proxyHostName->setText(proxy.hostName());
    ui->proxyPort->setValue(proxy.port());
    ui->proxyUserName->setText(proxy.user());
    ui->proxyPassword->setText(proxy.password());

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
}

void ProxySettingsWidget::saveSettings()
{
    QNetworkProxy proxy;
    bool ok;

    qDebug() << ui->proxyType->currentIndex();
    if (ui->proxySupport->isChecked()) {
        switch(ui->proxyType->currentIndex()) {
        case 0:
            proxy.setType(QNetworkProxy::Socks5Proxy);
            break;
        case 1:
            proxy.setType(QNetworkProxy::HttpCachingProxy);
            break;
        case 2:
            proxy.setType(QNetworkProxy::HttpProxy);
            break;
        default:
            proxy.setType(QNetworkProxy::DefaultProxy);
        }
    }
    else
        proxy.setType(QNetworkProxy::NoProxy);

    proxy.setHostName(ui->proxyHostName->text());
    int port = ui->proxyPort->text().toInt(&ok);
    if (ok)
        proxy.setPort(port);

    proxy.setUser(ui->proxyUserName->text());
    proxy.setPassword(ui->proxyPassword->text());

    QNetworkProxy::setApplicationProxy(proxy);

    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(false);
}

ProxySettingsPage::ProxySettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("Proxy"), QLatin1String("WebView"), parent)
{
}

QWidget *ProxySettingsPage::createPage(QWidget *parent)
{
    return new ProxySettingsWidget(parent);
}
