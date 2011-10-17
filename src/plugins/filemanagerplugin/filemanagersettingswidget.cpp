#include "filemanagersettingswidget.h"
#include "ui_filemanagersettingswidget.h"

#include <QtCore/QSignalMapper>
#include <core.h>
#include <settings.h>

#include "navigationmodel.h"

using namespace CorePlugin;

static int readFlags()
{
    int flags = 0;

    Settings *s = Core::instance()->settings();
    s->beginGroup(QLatin1String("fileManager"));
    if (s->contains(QLatin1String("standardLocations"))) {
        flags = s->value(QLatin1String("standardLocations")).toInt();
    } else {
        flags = NavigationModel::DesktopLocation |
                NavigationModel::DocumentsLocation |
                NavigationModel::HomeLocation |
                NavigationModel::ApplicationsLocation;
    }
    s->endGroup();

    return flags;
}

FileManagerSettingsWidget::FileManagerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileManagerSettingsWidget)
{
    ui->setupUi(this);

    setupLeftPanel();
    setupIconSize();
    setupGridSize();
    setupFlow();
}

FileManagerSettingsWidget::~FileManagerSettingsWidget()
{
    delete ui;
}

void FileManagerSettingsWidget::onChecked(bool checked)
{
    int flags = readFlags();

    if (checked)
        flags = flags | sender()->property("flag").toInt();
    else
        flags = flags & ~sender()->property("flag").toInt();

    Settings *s = Core::instance()->settings();
    s->beginGroup(QLatin1String("fileManager"));
    s->setValue(QLatin1String("standardLocations"), flags);
    s->endGroup();
}

void FileManagerSettingsWidget::onIconSizeChanged(int value)
{
    value *= 4;
    QSize size(value, value);
    ui->iconSizeLabel->setText(tr("Icon size: %1x%2").arg(value).arg(value));

    Settings *s = Core::instance()->settings();
    s->setValue(QLatin1String("fileManager/iconSize"), size);
}

void FileManagerSettingsWidget::onGridSizeChanged(int value)
{
    value *= 4;
    QSize size(value, value);
    ui->gridSizeLabel->setText(tr("Grid size: %1x%2").arg(value).arg(value));

    Settings *s = Core::instance()->settings();
    s->setValue(QLatin1String("fileManager/gridSize"), size);
}

void FileManagerSettingsWidget::onFlowChanged(int value)
{
    Settings *s = Core::instance()->settings();
    s->setValue(QLatin1String("fileManager/flow"), value);
}

void FileManagerSettingsWidget::setupLeftPanel()
{
    int flags = readFlags();

    ui->applicationsCheckBox->setChecked(flags & NavigationModel::ApplicationsLocation);
    ui->desktopCheckBox->setChecked(flags & NavigationModel::DesktopLocation);
    ui->documentsCheckBox->setChecked(flags & NavigationModel::DocumentsLocation);
    ui->downloadsCheckBox->setChecked(flags & NavigationModel::DownloadsLocation);
    ui->moviesCheckBox->setChecked(flags & NavigationModel::MoviesLocation);
    ui->musicCheckBox->setChecked(flags & NavigationModel::MusicLocation);
    ui->picturesCheckBox->setChecked(flags & NavigationModel::PicturesLocation);
    ui->homeCheckBox->setChecked(flags & NavigationModel::HomeLocation);

    ui->applicationsCheckBox->setProperty("flag", NavigationModel::ApplicationsLocation);
    ui->desktopCheckBox->setProperty("flag", NavigationModel::DesktopLocation);
    ui->documentsCheckBox->setProperty("flag", NavigationModel::DocumentsLocation);
    ui->downloadsCheckBox->setProperty("flag", NavigationModel::DownloadsLocation);
    ui->moviesCheckBox->setProperty("flag", NavigationModel::MoviesLocation);
    ui->musicCheckBox->setProperty("flag", NavigationModel::MusicLocation);
    ui->picturesCheckBox->setProperty("flag", NavigationModel::PicturesLocation);
    ui->homeCheckBox->setProperty("flag", NavigationModel::HomeLocation);

    connect(ui->applicationsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->desktopCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->documentsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->downloadsCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->moviesCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->musicCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->picturesCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
    connect(ui->homeCheckBox, SIGNAL(toggled(bool)), SLOT(onChecked(bool)));
}

void FileManagerSettingsWidget::setupIconSize()
{
    Settings *s = Core::instance()->settings();

    int iconSize = s->value(QLatin1String("fileManager/iconSize")).toSize().height();
    if (iconSize > 0) {
#ifdef Q_OS_MAC
        iconSize = 64;
#else
        iconSize = 32;
#endif
    }
    ui->iconSizeSlider->setValue(iconSize/4);
    ui->iconSizeLabel->setText(tr("Icon size: %1x%2").arg(iconSize).arg(iconSize));

    connect(ui->iconSizeSlider, SIGNAL(valueChanged(int)), SLOT(onIconSizeChanged(int)));
}

void FileManagerSettingsWidget::setupGridSize()
{
    Settings *s = Core::instance()->settings();

    int gridSize = s->value(QLatin1String("fileManager/gridSize")).toSize().height();
    if (gridSize == 0) {
#ifdef Q_OS_MAC
         gridSize = 128;
#else
         gridSize = 100;
#endif
    }
    ui->gridSizeLabel->setText(tr("Grid size: %1x%2").arg(gridSize).arg(gridSize));
    ui->gridSizeSlider->setValue(gridSize/4);


    connect(ui->gridSizeSlider, SIGNAL(valueChanged(int)), SLOT(onGridSizeChanged(int)));
}

void FileManagerSettingsWidget::setupFlow()
{
    Settings *s = Core::instance()->settings();

    ui->flowComboBox->setCurrentIndex(s->value(QLatin1String("fileManager/flow")).toInt());
    connect(ui->flowComboBox, SIGNAL(currentIndexChanged(int)), SLOT(onFlowChanged(int)));
}
