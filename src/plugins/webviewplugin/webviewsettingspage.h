#ifndef WEBVIEWSETTINGSPAGE_H
#define WEBVIEWSETTINGSPAGE_H

#include <coreplugin/isettingspage.h>

class WebViewSettingsPage : public CorePlugin::ISettingsPage
{
    Q_OBJECT
public:
    explicit WebViewSettingsPage(QObject *parent = 0);

    QString name() const;
    QIcon icon() const;

    QString categoryName() const;
    QIcon categoryIcon() const;

    QWidget *createPage(QWidget *parent);
};

#endif // WEBVIEWSETTINGSPAGE_H
