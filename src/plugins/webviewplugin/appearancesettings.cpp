#include "appearancesettings.h"
#include "ui_appearancesettings.h"

#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtGui/QFontDialog>
#include <QtWebKit/QWebSettings>

AppearanceSettingsWidget::AppearanceSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppearanceSettingsWidget),
    m_webSettings(QWebSettings::globalSettings())
{
    ui->setupUi(this);

    loadDefaults();
    loadSettings();

    connect(ui->fixedFontButton, SIGNAL(clicked()), SLOT(selectFixedWidthFont()));
    connect(ui->standardFontButton, SIGNAL(clicked()), SLOT(selectStandardFont()));
    connect(ui->minimFontSizeCheckBox, SIGNAL(toggled(bool)), SLOT(toggleMinimumFontSize(bool)));
    connect(ui->minimumFontSizeSpinBox, SIGNAL(valueChanged(int)), SLOT(setMinimumFontSize(int)));
    connect(ui->defaultEncoding, SIGNAL(currentIndexChanged(int)), SLOT(encodingChanged(int)));
}

AppearanceSettingsWidget::~AppearanceSettingsWidget()
{
    delete ui;
}

void AppearanceSettingsWidget::setFixedWidthFont(const QFont &font)
{
    m_fixedFont = font;

    m_webSettings->setFontFamily(QWebSettings::FixedFont, m_fixedFont.family());
    m_webSettings->setFontSize(QWebSettings::DefaultFixedFontSize, m_fixedFont.pointSize());

    ui->fixedFont->setText(QString(QLatin1String("%1 %2")).
                           arg(m_fixedFont.family()).
                           arg(m_fixedFont.pointSize()));
}

void AppearanceSettingsWidget::setStandardFont(const QFont &font)
{
    m_standardFont = font;

    m_webSettings->setFontFamily(QWebSettings::StandardFont, m_standardFont.family());
    m_webSettings->setFontSize(QWebSettings::DefaultFontSize, m_standardFont.pointSize());

    ui->standardFont->setText(QString(QLatin1String("%1 %2")).
                              arg(m_standardFont.family()).
                              arg(m_standardFont.pointSize()));
}

void AppearanceSettingsWidget::selectFixedWidthFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_fixedFont, this);
    if (ok)
        setFixedWidthFont(font);
}

void AppearanceSettingsWidget::toggleMinimumFontSize(bool on)
{
    ui->minimumFontSizeSpinBox->setEnabled(on);
    int minimumFontSize = on ? ui->minimumFontSizeSpinBox->value() : 0;
    if (!on)
        ui->minimumFontSizeSpinBox->setValue(9);
    m_webSettings->setFontSize(QWebSettings::MinimumFontSize, minimumFontSize);
}

void AppearanceSettingsWidget::setMinimumFontSize(int minimumFontSize)
{
    m_webSettings->setFontSize(QWebSettings::MinimumFontSize, minimumFontSize);
}

void AppearanceSettingsWidget::encodingChanged(int index)
{
    QByteArray codec = m_codecNames[index];
    m_webSettings->setDefaultTextEncoding(codec);
}

void AppearanceSettingsWidget::selectStandardFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, m_standardFont, this);
    if (ok)
        setStandardFont(font);
}

void AppearanceSettingsWidget::loadCodecs()
{
    QList<int> mibs = QTextCodec::availableMibs();
    foreach (int mib, mibs) {
        QTextCodec *codec = QTextCodec::codecForMib(mib);
        m_codecNames.append(codec->name());
    }
    qSort(m_codecNames);
}

void AppearanceSettingsWidget::loadDefaults()
{
    QString fixedFontFamily = m_webSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = m_webSettings->fontSize(QWebSettings::DefaultFixedFontSize);

    QString standardFontFamily = m_webSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = m_webSettings->fontSize(QWebSettings::DefaultFontSize);

    m_fixedFont = QFont(fixedFontFamily, fixedFontSize);
    m_standardFont = QFont(standardFontFamily, standardFontSize);

    loadCodecs();
}

void AppearanceSettingsWidget::loadSettings()
{
    QString fixedFontFamily = m_webSettings->fontFamily(QWebSettings::FixedFont);
    int fixedFontSize = m_webSettings->fontSize(QWebSettings::DefaultFixedFontSize);

    QString standardFontFamily = m_webSettings->fontFamily(QWebSettings::StandardFont);
    int standardFontSize = m_webSettings->fontSize(QWebSettings::DefaultFontSize);

    m_fixedFont = QFont(fixedFontFamily, fixedFontSize);
    ui->fixedFont->setText(QString(QLatin1String("%1 %2")).
                           arg(m_fixedFont.family()).
                           arg(m_fixedFont.pointSize()));

    m_standardFont = QFont(standardFontFamily, standardFontSize);
    ui->standardFont->setText(QString(QLatin1String("%1 %2")).
                              arg(m_standardFont.family()).
                              arg(m_standardFont.pointSize()));

    int minimumFontSize = m_webSettings->fontSize(QWebSettings::MinimumFontSize);
    ui->minimFontSizeCheckBox->setChecked(minimumFontSize != 0);
    ui->minimumFontSizeSpinBox->setEnabled(minimumFontSize != 0);
    ui->minimumFontSizeSpinBox->setValue(minimumFontSize ? minimumFontSize : 9);

    foreach (const QByteArray &codec, m_codecNames) {
        ui->defaultEncoding->addItem(codec);
    }

    QByteArray defaultEncoding;
    defaultEncoding.append(m_webSettings->defaultTextEncoding());
    int index = m_codecNames.indexOf(defaultEncoding);
    ui->defaultEncoding->setCurrentIndex(index);
}

AppearanceSettingsPage::AppearanceSettingsPage(QObject *parent) :
    SettingsPage(QLatin1String("Appearance"), QLatin1String("WebView"), parent)
{
}

QWidget * AppearanceSettingsPage::createPage(QWidget *parent)
{
    return new AppearanceSettingsWidget(parent);
}
