#include "qimageviewpreferenceswidget.h"
#include "ui_qimageviewpreferenceswidget.h"

#include "qimageviewsettings.h"

QImageViewPreferencesWidget::QImageViewPreferencesWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QImageViewPreferencesWidget)
{
    ui->setupUi(this);

    QImageViewSettings *settings = QImageViewSettings::globalSettings();

    QImageViewSettings::ImageBackgroundType type = settings->imageBackgroundType();

    switch (type) {
    case QImageViewSettings::None:
        ui->noBackgroundButton->setChecked(true);
        break;
    case QImageViewSettings::SolidColor:
        ui->solidColorRadioButton->setChecked(true);
        break;
    case QImageViewSettings::Chess:
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

QImageViewPreferencesWidget::~QImageViewPreferencesWidget()
{
    delete ui;
}

void QImageViewPreferencesWidget::noBackgroundClicked(bool clicked)
{
    if (clicked)
        QImageViewSettings::globalSettings()->setiImageBackgroundType(QImageViewSettings::None);
}

void QImageViewPreferencesWidget::solidColorClicked(bool clicked)
{
    if (clicked)
        QImageViewSettings::globalSettings()->setiImageBackgroundType(QImageViewSettings::SolidColor);
}

void QImageViewPreferencesWidget::checkeredBackgroundClicked(bool clicked)
{
    if (clicked)
        QImageViewSettings::globalSettings()->setiImageBackgroundType(QImageViewSettings::Chess);
}

void QImageViewPreferencesWidget::backgroundColorChanged(const QColor &color)
{
    QImageViewSettings::globalSettings()->setBackgroundColor(color);
}

void QImageViewPreferencesWidget::imageColorChanged(const QColor &color)
{
    QImageViewSettings::globalSettings()->setImageBackgroundColor(color);
}

void QImageViewPreferencesWidget::useOpenGLClicked(bool clicked)
{
    QImageViewSettings::globalSettings()->setUseOpenGL(clicked);
}
