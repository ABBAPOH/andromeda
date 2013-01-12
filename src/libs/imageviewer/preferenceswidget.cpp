#include "preferenceswidget.h"
#include "ui_preferenceswidget.h"

#include "imageviewsettings.h"

using namespace ImageViewer;

PreferencesWidget::PreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreferencesWidget)
{
    ui->setupUi(this);

    ImageViewSettings *settings = ImageViewSettings::globalSettings();

    ImageViewSettings::ImageBackgroundType type = settings->imageBackgroundType();

    switch (type) {
    case ImageViewSettings::None:
        ui->noBackgroundButton->setChecked(true);
        break;
    case ImageViewSettings::SolidColor:
        ui->solidColorRadioButton->setChecked(true);
        break;
    case ImageViewSettings::Chess:
        ui->checkedredBackgroundButton->setChecked(true);
        break;
    default:
        break;
    }

    connect(ui->noBackgroundButton, SIGNAL(clicked(bool)), SLOT(noBackgroundClicked(bool)));
    connect(ui->solidColorRadioButton, SIGNAL(clicked(bool)), SLOT(solidColorClicked(bool)));
    connect(ui->checkedredBackgroundButton, SIGNAL(clicked(bool)), SLOT(checkeredBackgroundClicked(bool)));

    QColor backgroundColor = settings->backgroundColor();
    ui->backgroundButton->setColor(backgroundColor);

    QColor imageColor = settings->imageBackgroundColor();
    ui->solidColorButton->setColor(imageColor);

    connect(ui->backgroundButton, SIGNAL(colorChanged(QColor)), SLOT(backgroundColorChanged(QColor)));
    connect(ui->solidColorButton, SIGNAL(colorChanged(QColor)), SLOT(imageColorChanged(QColor)));

    ui->useOpenGL->setChecked(settings->useOpenGL());
    connect(ui->useOpenGL, SIGNAL(clicked(bool)), SLOT(useOpenGLClicked(bool)));
}

PreferencesWidget::~PreferencesWidget()
{
    delete ui;
}

void PreferencesWidget::noBackgroundClicked(bool clicked)
{
    if (clicked)
        ImageViewSettings::globalSettings()->setiImageBackgroundType(ImageViewSettings::None);
}

void PreferencesWidget::solidColorClicked(bool clicked)
{
    if (clicked)
        ImageViewSettings::globalSettings()->setiImageBackgroundType(ImageViewSettings::SolidColor);
}

void PreferencesWidget::checkeredBackgroundClicked(bool clicked)
{
    if (clicked)
        ImageViewSettings::globalSettings()->setiImageBackgroundType(ImageViewSettings::Chess);
}

void PreferencesWidget::backgroundColorChanged(const QColor &color)
{
    ImageViewSettings::globalSettings()->setBackgroundColor(color);
}

void PreferencesWidget::imageColorChanged(const QColor &color)
{
    ImageViewSettings::globalSettings()->setImageBackgroundColor(color);
}

void PreferencesWidget::useOpenGLClicked(bool clicked)
{
    ImageViewSettings::globalSettings()->setUseOpenGL(clicked);
}
