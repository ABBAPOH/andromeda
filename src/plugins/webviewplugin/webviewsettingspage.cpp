#include "webviewsettingspage.h"

#include <QtGui/QFileIconProvider>
#include <QtGui/QLabel>

WebViewSettingsPage::WebViewSettingsPage(QObject *parent) :
    CorePlugin::ISettingsPage(QLatin1String("WebView"), QLatin1String("WebView"), parent)
{
}

QString WebViewSettingsPage::name() const
{
    return tr("Global");
}

QIcon WebViewSettingsPage::icon() const

{
    return QFileIconProvider().icon(QFileIconProvider::Network);
}

QString WebViewSettingsPage::categoryName() const
{
    return tr("Web View");
}

QIcon WebViewSettingsPage::categoryIcon() const
{
    return icon();
}

QWidget * WebViewSettingsPage::createPage(QWidget *parent)
{
    QLabel *label = new QLabel("Not implemented", parent);
    label->setAlignment(Qt::AlignCenter);
    return label;
}
