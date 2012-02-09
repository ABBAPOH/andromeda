#include "privacysettings.h"
#include "ui_privacysettings.h"

#include <QtCore/QSettings>
#include <QtCore/QMetaEnum>
#include <QtWebKit/QWebSettings>

#include "cookiejar.h"
#include "cookiedialog.h"
#include "cookieexceptionsdialog.h"

PrivacySettingsWidget::PrivacySettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivacySettingsWidget),
    m_settings(new QSettings(this)),
    m_webSettings(QWebSettings::globalSettings()),
    m_cookieJar(0)
{
    ui->setupUi(this);

    m_settings->beginGroup("websettings");
    m_settings->beginGroup("privacy");

    connect(ui->blockPopupWindows, SIGNAL(toggled(bool)), SLOT(setBlockPopupWindows(bool)));
    connect(ui->enableJavascript, SIGNAL(toggled(bool)), SLOT(setJavascriptEnabled(bool)));
    connect(ui->enableLocalStorage, SIGNAL(toggled(bool)), SLOT(setLocalStorageEnabled(bool)));
    connect(ui->enableImages, SIGNAL(toggled(bool)), SLOT(setImagesEnabled(bool)));
    connect(ui->enablePlugins, SIGNAL(toggled(bool)), SLOT(setPluginsEnabled(bool)));

    connect(ui->acceptCombo, SIGNAL(currentIndexChanged(int)), SLOT(setAcceptPolicy(int)));
    connect(ui->keepUntilCombo, SIGNAL(currentIndexChanged(int)), SLOT(setKeepPolicy(int)));
    connect(ui->cookieSessionCombo, SIGNAL(currentIndexChanged(int)), SLOT(setSessionLength(int)));
    connect(ui->filterTrackingCookiesCheckbox, SIGNAL(toggled(bool)), SLOT(setFilterTrackingCookies(bool)));

    connect(ui->cookiesButton, SIGNAL(clicked()), this, SLOT(showCookies()));
    connect(ui->exceptionsButton, SIGNAL(clicked()), this, SLOT(showExceptions()));

    loadSettings();
}

PrivacySettingsWidget::~PrivacySettingsWidget()
{
    delete ui;
}

CookieJar *PrivacySettingsWidget::cookieJar() const
{
    return m_cookieJar;
}

void PrivacySettingsWidget::setCookieJar(CookieJar *jar)
{
    if (m_cookieJar == jar)
        return;

    m_cookieJar = jar;
    loadCookieSettings();
}

void PrivacySettingsWidget::setBlockPopupWindows(bool yes)
{
    m_settings->setValue(QLatin1String("blockPopupWindows"), yes);
    m_webSettings->setAttribute(QWebSettings::JavascriptCanOpenWindows, yes);
}

void PrivacySettingsWidget::setJavascriptEnabled(bool yes)
{
    m_settings->setValue(QLatin1String("javascriptEnabled"), yes);
    m_webSettings->setAttribute(QWebSettings::JavascriptEnabled, yes);
}

void PrivacySettingsWidget::setImagesEnabled(bool yes)
{
    m_settings->setValue(QLatin1String("enableImages"), yes);
    m_webSettings->setAttribute(QWebSettings::AutoLoadImages, yes);
}

void PrivacySettingsWidget::setLocalStorageEnabled(bool yes)
{
    m_settings->setValue(QLatin1String("enableLocalStorage"), yes);
    m_webSettings->setAttribute(QWebSettings::LocalStorageEnabled, yes);
}

void PrivacySettingsWidget::setPluginsEnabled(bool yes)
{
    m_settings->setValue(QLatin1String("enablePlugins"), yes);
    m_webSettings->setAttribute(QWebSettings::PluginsEnabled, yes);
}

void PrivacySettingsWidget::setAcceptPolicy(int policy)
{
    CookieJar::AcceptPolicy acceptPolicy = CookieJar::AcceptAlways;
    switch (policy) {
    case 0:
        acceptPolicy = CookieJar::AcceptAlways;
        break;
    case 1:
        acceptPolicy = CookieJar::AcceptNever;
        break;
    case 2:
        acceptPolicy = CookieJar::AcceptOnlyFromSitesNavigatedTo;
        break;
    default:
        break;
    }

    m_cookieJar->setAcceptPolicy(acceptPolicy);
}

void PrivacySettingsWidget::setKeepPolicy(int policy)
{
    CookieJar::KeepPolicy keepPolicy = CookieJar::KeepUntilExpire;
    switch (policy) {
    case 0:
        keepPolicy = CookieJar::KeepUntilExpire;
        break;
    case 1:
        keepPolicy = CookieJar::KeepUntilExit;
        break;
    case 2:
        keepPolicy = CookieJar::KeepUntilTimeLimit;
        break;
    default:
        break;
    }

    m_cookieJar->setKeepPolicy(keepPolicy);
}

void PrivacySettingsWidget::setSessionLength(int index)
{
    int sessionLength = 0;
    switch (index) {
    case 1: sessionLength = 1; break;
    case 2: sessionLength = 2; break;
    case 3: sessionLength = 3; break;
    case 4: sessionLength = 7; break;
    case 5: sessionLength = 30; break;
    default:
    case 0: sessionLength = -1; break;
    }

    m_cookieJar->setSessionLength(sessionLength);
}

void PrivacySettingsWidget::setFilterTrackingCookies(bool filter)
{
    if (!m_cookieJar)
        return;

    m_cookieJar->setFilterTrackingCookies(filter);
}

void PrivacySettingsWidget::showCookies()
{
    if (!m_cookieJar)
        return;

    CookieDialog dialog;
    dialog.setCookieJar(m_cookieJar);
    dialog.exec();
}

void PrivacySettingsWidget::showExceptions()
{
    if (!m_cookieJar)
        return;

    CookieExceptionsDialog dialog;
    dialog.setCookieJar(m_cookieJar);
    dialog.exec();
}

void PrivacySettingsWidget::loadSettings()
{
    bool blockPopupWindows = m_settings->value(QLatin1String("blockPopupWindows"), true).toBool();
    bool enableImages = m_settings->value(QLatin1String("enableImages"), true).toBool();
    bool enableJavascript = m_settings->value(QLatin1String("javascriptEnabled"), true).toBool();
    bool enableLocalStorage = m_settings->value(QLatin1String("enableLocalStorage"), true).toBool();
    bool enablePlugins = m_settings->value(QLatin1String("enablePlugins"), true).toBool();

    m_webSettings->setAttribute(QWebSettings::JavascriptCanOpenWindows, blockPopupWindows);
    m_webSettings->setAttribute(QWebSettings::AutoLoadImages, enableImages);
    m_webSettings->setAttribute(QWebSettings::JavascriptEnabled, enableJavascript);
    m_webSettings->setAttribute(QWebSettings::LocalStorageEnabled, enableLocalStorage);
    m_webSettings->setAttribute(QWebSettings::PluginsEnabled, enablePlugins);

    ui->blockPopupWindows->setChecked(blockPopupWindows);
    ui->enableImages->setChecked(enableImages);
    ui->enableJavascript->setChecked(enableJavascript);
    ui->enableLocalStorage->setChecked(enableLocalStorage);
    ui->enablePlugins->setChecked(enablePlugins);
}

void PrivacySettingsWidget::loadCookieSettings()
{
    if (!m_cookieJar)
        return;

    CookieJar::AcceptPolicy acceptPolicy = m_cookieJar->acceptPolicy();
    CookieJar::KeepPolicy keepPolicy = m_cookieJar->keepPolicy();
    int sessionLength = m_cookieJar->sessionLength();
    bool filterTrackingCookies = m_cookieJar->filterTrackingCookies();

    switch (acceptPolicy) {
    case CookieJar::AcceptAlways:
        ui->acceptCombo->setCurrentIndex(0);
        break;
    case CookieJar::AcceptNever:
        ui->acceptCombo->setCurrentIndex(1);
        break;
    case CookieJar::AcceptOnlyFromSitesNavigatedTo:
        ui->acceptCombo->setCurrentIndex(2);
        break;
    }

    switch (keepPolicy) {
    case CookieJar::KeepUntilExpire:
        ui->keepUntilCombo->setCurrentIndex(0);
        break;
    case CookieJar::KeepUntilExit:
        ui->keepUntilCombo->setCurrentIndex(1);
        break;
    case CookieJar::KeepUntilTimeLimit:
        ui->keepUntilCombo->setCurrentIndex(2);
        break;
    }

    switch (sessionLength) {
    case 1: ui->cookieSessionCombo->setCurrentIndex(1); break;
    case 2: ui->cookieSessionCombo->setCurrentIndex(2); break;
    case 3: ui->cookieSessionCombo->setCurrentIndex(3); break;
    case 7: ui->cookieSessionCombo->setCurrentIndex(4); break;
    case 30: ui->cookieSessionCombo->setCurrentIndex(5); break;
    default:
    case 0: ui->cookieSessionCombo->setCurrentIndex(0); break;
    }

    ui->filterTrackingCookiesCheckbox->setChecked(filterTrackingCookies);
}
