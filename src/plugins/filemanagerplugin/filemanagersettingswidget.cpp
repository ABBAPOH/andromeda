#include "filemanagersettingswidget.h"
#include "ui_filemanagersettingswidget.h"

#include <QtCore/QSignalMapper>
#include <core.h>
#include <settings.h>

#include "navigationmodel.h"

using namespace CorePlugin;

FileManagerSettingsWidget::FileManagerSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileManagerSettingsWidget)
{
    ui->setupUi(this);

    Settings *s = Core::instance()->settings();
    s->beginGroup("fileManager");
    int flags = s->value(QLatin1String("standardLocations")).toInt();
    s->endGroup();

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

FileManagerSettingsWidget::~FileManagerSettingsWidget()
{
    delete ui;
}

void FileManagerSettingsWidget::onChecked(bool checked)
{
    Settings *s = Core::instance()->settings();
    s->beginGroup("fileManager");
    int flags = 0;
    if (s->contains(QLatin1String("standardLocations"))) {
        flags = s->value(QLatin1String("standardLocations")).toInt();
    } else {
        flags = NavigationModel::DesktopLocation |
                NavigationModel::DocumentsLocation |
                NavigationModel::HomeLocation |
                NavigationModel::ApplicationsLocation;
    }
    if (checked)
        flags = flags | sender()->property("flag").toInt();
    else
        flags = flags & ~sender()->property("flag").toInt();
    s->setValue(QLatin1String("standardLocations"), flags);
    s->endGroup();
}
