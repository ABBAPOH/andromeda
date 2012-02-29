#ifndef APPEARANCESETTINGS_H
#define APPEARANCESETTINGS_H

#include <QtGui/QWidget>
#include <QtGui/QFileIconProvider>

class QSettings;
class QWebSettings;

namespace Ui {
class AppearanceSettingsWidget;
}

class AppearanceSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppearanceSettingsWidget(QWidget *parent = 0);
    ~AppearanceSettingsWidget();

public slots:
    void setStandardFont(const QFont &font);
    void setFixedWidthFont(const QFont &font);

private slots:
    void selectStandardFont();
    void selectFixedWidthFont();
    void toggleMinimumFontSize(bool);
    void setMinimumFontSize(int);
    void encodingChanged(int index);

private:
    void loadCodecs();
    void loadDefaults();
    void loadSettings();

private:
    Ui::AppearanceSettingsWidget *ui;
    QWebSettings *m_webSettings;
    QFont m_standardFont;
    QFont m_fixedFont;
    QList<QByteArray> m_codecNames;
};

#include <guisystem/settingspage.h>

class AppearanceSettingsPage : public GuiSystem::SettingsPage
{
    Q_OBJECT

public:
    explicit AppearanceSettingsPage(QObject *parent = 0);

    QString name() const { return tr("Appearance"); }
    QIcon icon() const { return QIcon(); }

    QString categoryName() const { return tr("Web view"); }
    QIcon categoryIcon() const { return QFileIconProvider().icon(QFileIconProvider::Network); }

    QWidget *createPage(QWidget *parent);

};

#endif // APPEARANCESETTINGS_H
